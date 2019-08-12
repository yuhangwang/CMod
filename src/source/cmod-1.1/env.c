/* cmod - set up the user environment with module files.
   Copyright (C) 1997, 1998 by the authors of cmod (see AUTHORS).

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <config.h>

#include <assert.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#include <string.h>
#include <sys/types.h>

#include "cmod_string.h"
#include "env.h"
#include "cmod_main.h"
#include "cmod_trace.h"
#include "cmod_alloc.h"
#include "archaic.h"

struct env {
  /* Name of the environment variable. */
  struct cmod_str name;

  /* True if the environment variable is unset. */
  int unset;

  /* True if the environment variable has been read from the real
     environment and not modified since. */
  int clean;

  /* True if key_and_val is valid. */
  int kv_valid;

  /* The current delimiter used to separate the fragments in frags.
     This is only valid when nfrags is non-zero. */
  char current_delimiter;

  /* Number of used fragments in frags, or 0 if frags is not valid. */
  int nfrags;

  /* The fragments. */
  struct cmod_str *frags;

  /* Number of allocated fragments in frags.  (Only the first nfrags
     elements of the frags array contain valid data, but all of them
     are properly constructed strings.) */
  int frags_allocated;

  /* True if non_fragmented is valid. */
  int non_fragmented_valid;

  /* The unfragmented value. */
  struct cmod_str non_fragmented;

  /* Name and value in a form suitable to building up an environ structure. */
  struct cmod_str key_and_val;
};


struct envarray {
  /* Number of objects in ARR. */
  int size;

  /* The list of variables. */
  struct env *arr;
};

#ifdef NDEBUG
#define env_sane(x) do { } while (0)
#else

static void
assert_kv(const struct cmod_str	*k,
	  const struct cmod_str	*v,
	  const struct cmod_str	*kv)
{
  int ix;
  const char *a;
  const char *b;

  assert(str_strlen(k) + str_strlen(v) + 1 == str_strlen(kv));

  a = str_asciz(k);
  b = str_asciz(kv);

  for (ix = str_strlen(k); ix > 0; --ix)
    {
      assert(*a == *b);
      ++a;
      ++b;
    }

  assert(*b == '=');
  ++b;
  a = str_asciz(v);
  for (ix = str_strlen(v); ix > 0; --ix)
    {
      assert(*a == *b);
      ++a;
      ++b;
    }
  assert(*b == '\0');
}

  
/* Check that the "struct env" is sane. */
static void
env_sane(struct env *li)
{
  int i;
  struct cmod_str defrag;

  assert(li->clean == 0 || li->clean == 1);
  assert(li->unset == 0 || li->unset == 1);
  assert(li->kv_valid == 0 || li->kv_valid == 1);
  if (li->unset)
    {
      assert(li->nfrags == 0);
      assert(li->non_fragmented_valid == 0);
      assert(li->kv_valid == 0);
    }
  else
    {
      if (li->nfrags > 0)
	assert(li->current_delimiter != '\0');

      if (li->nfrags > 0 && (li->non_fragmented_valid || li->kv_valid))
	{
	  str_ctor(&defrag);
	  for (i = 0; i < li->nfrags; ++i)
	    {
	      if (i)
		str_append_buf(&defrag, &li->current_delimiter, 1);
	      str_append_str(&defrag, &li->frags[i]);
	    }
	  if (li->non_fragmented_valid)
	    {
	      assert(!strcmp(str_asciz(&defrag),
			     str_asciz(&li->non_fragmented)));
	    }
	  if (li->kv_valid)
	    assert_kv(&li->name, &defrag, &li->key_and_val);
	  str_dtor(&defrag);
	}
      if (li->kv_valid && li->non_fragmented_valid)
	assert_kv(&li->name, &li->non_fragmented, &li->key_and_val);
    }
}

#endif /* NDEBUG */

static void
env_ctor(struct env *li)
{
  str_ctor(&li->name);
  li->unset = 0;
  li->clean = 0;
  li->kv_valid = 0;
  li->current_delimiter = 0;
  li->nfrags = 0;
  li->frags = NULL;
  li->frags_allocated = 0;
  li->non_fragmented_valid = 0;
  str_ctor(&li->non_fragmented);
  str_ctor(&li->key_and_val);
  env_sane(li);
}


static void
env_dtor(struct env *li)
{
  int i;

  env_sane(li);

  str_dtor(&li->name);
  for (i = 0; i < li->frags_allocated; ++i)
    str_dtor(&li->frags[i]);
  if (li->frags != NULL)
    xfree(li->frags);
  str_dtor(&li->non_fragmented);
  str_dtor(&li->key_and_val);
}


static void
env_invalidate_unfragmented(struct env *li)
{
  li->non_fragmented_valid = 0;
  str_clear(&li->non_fragmented);
}

static void
env_invalidate_fragmented(struct env *li)
{
  int i;

  for (i = 0; i < li->nfrags; ++i)
    str_clear(&li->frags[i]);
  li->current_delimiter = '\0';
  li->nfrags = 0;
}


static void
env_defragment(struct env *li)
{
  int ix;

  if (li->non_fragmented_valid || li->unset)
    return;

  str_clear(&li->non_fragmented);
  for (ix = 0; ix < li->nfrags; ++ix)
    {
      if (ix)
	str_append_char(&li->non_fragmented, li->current_delimiter);
      str_append_str(&li->non_fragmented, &li->frags[ix]);
    }
  li->non_fragmented_valid = 1;
}


static void
require_fragments(struct env *li,
		  int nfrags)
{
  int ix;

  if (nfrags > li->frags_allocated)
    {
      li->frags = xrealloc(li->frags, nfrags * sizeof(struct cmod_str));
      for (ix = li->frags_allocated; ix < nfrags; ++ix)
	str_ctor(&li->frags[ix]);
      li->frags_allocated = nfrags;
    }
  for (ix = li->nfrags; ix < nfrags; ++ix)
    str_clear(&li->frags[ix]);
  li->nfrags = nfrags;
}


static void
env_fragment(struct env *li,
	     char frag_delimiter)
{
  int nfrags = 0;
  const char *frag;
  const char *frag_end;

  if (li->nfrags > 0 && li->current_delimiter == frag_delimiter)
    return;

  env_defragment(li);

  env_invalidate_fragmented(li);

  li->current_delimiter = frag_delimiter;

  if (li->non_fragmented_valid == 0 || str_empty(&li->non_fragmented))
    return;
  
  /* Count the number of fragments.  Uggliness: if ":" is the delimiter:
      ""    0 fragments (this case handled above)
      "x"   1 fragment: "x"
      ":"   2 fragments: "" and ""
      "x:y" 2 fragments: "x" and "y"
     As can be seen, there is no way to get one fragment containing
     the empty string.  */

  frag = str_asciz(&li->non_fragmented);
  if (frag != NULL && *frag != '\0')
    while (1)
      {
	++nfrags;		/* Count this frag. */
	frag = strchr(frag, frag_delimiter); /* Find next frag delimiter. */
	if (frag == NULL)	/* No more frags... */
	  break;		/* ...so exit the loop. */
	++frag;			/* Skip the delimiter. */
      }

  assert(nfrags > 0);
  require_fragments(li, nfrags);

  frag = str_asciz(&li->non_fragmented);
  assert(frag != NULL);

  for (nfrags = 0; nfrags < li->nfrags; ++nfrags)
    {
      frag_end = strchr(frag, frag_delimiter);
      if (frag_end == NULL)
	{
	  assert(nfrags == li->nfrags - 1);
	  str_append_asciz(&li->frags[nfrags], frag);
	}
      else
	{
	  assert(nfrags < li->nfrags - 1);
	  str_append_buf(&li->frags[nfrags], frag, frag_end - frag);
	}

      frag = frag_end + 1;
    }

  assert(nfrags == li->nfrags);
}


int
env_frag_present(struct env *li,
		 char frag_delimiter,
		 const struct cmod_str *frag)
{
  int i;

  env_sane(li);
  env_fragment(li, frag_delimiter);
  env_sane(li);

  for (i = 0; i < li->nfrags; ++i)
    if (!str_strcmp(&li->frags[i], frag))
      return 1;
  return 0;
}


void
env_append_one_frag(struct env *li,
		    char frag_delimiter,
		    const struct cmod_str *frag)
{
  assert(frag != NULL);
  assert(li != NULL);

  env_sane(li);
  env_fragment(li, frag_delimiter);

  require_fragments(li, li->nfrags + 1);
  str_append_str(&li->frags[li->nfrags - 1], frag);
  li->unset = 0;
  li->clean = 0;
  li->kv_valid = 0;

  env_invalidate_unfragmented(li);
  env_sane(li);

  trace(TRACE_ASSIGN, "appending %s << %s\n",
	str_asciz(&li->name), str_asciz(frag));
}


void
env_remove_one_frag(struct env *li,
		    char frag_delimiter,
		    const struct cmod_str *frag)
{
  int i;
  int move_i;

  assert(frag != NULL);
      
  env_sane(li);
  env_fragment(li, frag_delimiter);

  for (i = 0; i < li->nfrags; )
    if (!str_strcmp(&li->frags[i], frag))
      {
	trace(TRACE_ASSIGN, "removing %s >> %s\n",
	      str_asciz(&li->name), str_asciz(frag));
	env_invalidate_unfragmented(li);
	for (move_i = i; move_i < li->nfrags - 1; move_i++)
	  str_set_str(&li->frags[move_i], &li->frags[move_i + 1]);

	--li->nfrags;
	li->clean = 0;
	li->kv_valid = 0;
      }
    else
      ++i;

  env_sane(li);
}


void
env_append_frags(struct env *li,
		 char frag_delimiter,
		 const struct cmod_str *frags_in,
		 int unique)
{
  const char *frags;
  const char *frag_end;
  struct cmod_str copy;

  assert(frag_delimiter != '\0');

  env_sane(li);

  env_fragment(li, frag_delimiter);

  /* Ignore attempts to append an empty string. */
  if (str_empty(frags_in))
    {
      env_sane(li);
      return;
    }

  str_ctor(&copy);
  frags = str_asciz(frags_in);
  while (1)
    {
      frag_end = strchr(frags, frag_delimiter);
      str_clear(&copy);
      if (frag_end == NULL)
	{
	  str_append_asciz(&copy, frags);
	  if (!unique || !env_frag_present(li, frag_delimiter, &copy))
	    env_append_one_frag(li, frag_delimiter, &copy);
	  env_sane(li);
	  str_dtor(&copy);
	  return;
	}
      else
	{
	  str_append_buf(&copy, frags, frag_end - frags);
	  if (!unique || !env_frag_present(li, frag_delimiter, &copy))
	    env_append_one_frag(li, frag_delimiter, &copy);
	  frags = frag_end + 1;
	}
    }
}


void
env_prepend_one_frag(struct env *li,
		     char frag_delimiter,
		     struct cmod_str *frag)
{
  int i;

  trace(TRACE_ASSIGN, "prepending %s << %s\n", li->name, frag);

  env_sane(li);

  env_fragment(li, frag_delimiter);
  env_invalidate_unfragmented(li);

  require_fragments(li, li->nfrags + 1);

  for (i = li->nfrags - 2; i >= 0; i--)
    str_set_str(&li->frags[i + 1], &li->frags[i]);
	
  str_set_str(&li->frags[0], frag);
  li->unset = 0;
  li->clean = 0;
  li->kv_valid = 0;
  env_sane(li);
}


void
env_prepend_frags(struct env *li,
		  char frag_delimiter,
		  const struct cmod_str *frags,
		  int unique)
{
  int fragno;
  struct env parsed_frag;

  env_sane(li);

  env_ctor(&parsed_frag);
  str_append_asciz(&parsed_frag.name, "(internal)");
  env_append_frags(&parsed_frag, frag_delimiter, frags, unique);
  env_sane(&parsed_frag);

  for (fragno = parsed_frag.nfrags - 1; fragno >= 0 ; fragno--)
    {
      if (unique)
	env_remove_one_frag(li, frag_delimiter, &parsed_frag.frags[fragno]);

      env_prepend_one_frag(li, frag_delimiter, &parsed_frag.frags[fragno]);
    }

  env_dtor(&parsed_frag);
  env_sane(li);
}


void
env_remove_frags(struct env *li,
		 char frag_delimiter,
		 const struct cmod_str *frags_in)
{
  const char *frags;
  const char *frag_end;
  struct cmod_str copy;

  env_sane(li);
  assert(frag_delimiter != '\0');

  str_ctor(&copy);
  frags = str_asciz(frags_in);

  while (1)
    {
      frag_end = strchr(frags, frag_delimiter);
      str_clear(&copy);
      if (frag_end == NULL)
	{
	  str_append_asciz(&copy, frags);
	  env_remove_one_frag(li, frag_delimiter, &copy);
	  env_sane(li);
	  str_dtor(&copy);
	  return;
	}
      else
	{
	  str_append_buf(&copy, frags, frag_end - frags);
	  env_remove_one_frag(li, frag_delimiter, &copy);
	  frags = frag_end + 1;
	}
    }
}


void
env_setenv(struct env *li,
	   const struct cmod_str *val)
{
  trace(TRACE_ASSIGN, "setting %s == %s\n", li->name, val);

  env_sane(li);
  env_invalidate_fragmented(li);
  str_set_str(&li->non_fragmented, val);
  li->non_fragmented_valid = 1;
  li->unset = 0;
  li->clean = 0;
  li->kv_valid = 0;
  env_sane(li);
}


void
env_unsetenv(struct env *li)
{
  trace(TRACE_ASSIGN, "unsetting %s\n", li->name);

  env_sane(li);
  env_invalidate_fragmented(li);
  env_invalidate_unfragmented(li);
  li->unset = 1;
  li->clean = 0;
  li->kv_valid = 0;
  env_sane(li);
}

void
env_dump(struct env *li,
	 FILE *f,
	 const char *sh)
{
  const char *s;

  env_sane(li);

  env_defragment(li);

  if (!strcmp(sh, "sh"))
    {
      if (li->unset)
	fprintf(f, "unset %s;\n", str_asciz(&li->name));
      else
	{
	  fprintf(f, "%s=", str_asciz(&li->name));
	  for (s = str_asciz(&li->non_fragmented); *s != '\0'; ++s)
	    {
	      if (('a' <= *s && *s <= 'z') || ('A' <= *s && *s <= 'Z')
		  || ('0' <= *s && *s <= '9')
		  || *s == '/' || *s == '+' || *s == '-' || *s == '_'
		  || *s == ',' || *s == '.' || *s == ':')
		putc(*s, f);
	      else if (*s == '\n')
		{
		  putc('\'', f);
		  putc(*s, f);
		  putc('\'', f);
		}
	      else
		{
		  putc('\\', f);
		  putc(*s, f);
		}
	    }
	  fprintf(f, ";\nexport %s;\n", str_asciz(&li->name));
	}
    }
  else if (!strcmp(sh, "csh"))
    {
      if (li->unset)
	fprintf(f, "unsetenv %s;\n", str_asciz(&li->name));
      else
	{
	  fprintf(f, "setenv %s ", str_asciz(&li->name));
	  /* FIXME: this quoting is probably all wrong for csh. */
	  for (s = str_asciz(&li->non_fragmented); *s != '\0'; ++s)
	    {
	      if (('a' <= *s && *s <= 'z') || ('A' <= *s && *s <= 'Z')
		  || ('0' <= *s && *s <= '9')
		  || *s == '/' || *s == '+' || *s == '-' || *s == '_'
		  || *s == ',' || *s == '.' || *s == ':')
		putc(*s, f);
	      else if (*s == '\n')
		{
		  putc('\'', f);
		  putc(*s, f);
		  putc('\'', f);
		}
	      else
		{
		  putc('\\', f);
		  putc(*s, f);
		}
	    }
	  fputs(";\n", f);
	}
    }
  else
    {
      fprintf(stderr, "%s: Unsupported shell \"%s\"", argv0, sh);
      exit(1);
    }
  env_sane(li);
}


int
env_iterate(struct env *li,
	    struct cmod_str *frag_result,
	    char frag_delimiter,
	    int *iterator)
{
  env_sane(li);

  if (*iterator == 0)
    {
      env_fragment(li, frag_delimiter);
      env_sane(li);
    }
  else
    {
      assert(li->current_delimiter == frag_delimiter);
    }
  
  if (*iterator < li->nfrags)
    {
      str_clear(frag_result);
      str_append_str(frag_result, &li->frags[(*iterator)++]);
      return 1;
    }
  else
    return 0;
}


int
env_value(struct env *li, struct cmod_str *res)
{
  env_sane(li);
  env_defragment(li);
  str_set_str(res, &li->non_fragmented);
  return 2*li->unset + li->clean;
}


int
env_environ(struct env *li, const char **res)
{
  int retval = 2*li->unset + li->clean;
  env_sane(li);
  if (retval == 0)
    {
      if (!li->kv_valid)
	{
	  env_defragment(li);
	  str_set_str(&li->key_and_val, &li->name);
	  str_append_char(&li->key_and_val, '=');
	  str_append_str(&li->key_and_val, &li->non_fragmented);
	  li->kv_valid = 1;
	  env_sane(li);
	}
      *res = str_asciz(&li->key_and_val);
    }
  return retval;
}


struct env *
envarray_locate(struct envarray* arr,
		const struct cmod_str* name)
{
  int i;

  for (i = 0; i < arr->size; ++i)
    if (!str_strcmp(name, &arr->arr[i].name))
      return &arr->arr[i];

  return NULL;
}

struct env *
envarray_locate_create(struct envarray* arr,
		       const struct cmod_str *name)
{
  char *current_env = NULL;
  struct env *entry;
  struct cmod_str env_buf;

  if ((entry = envarray_locate(arr, name)) != NULL)
    return entry;

  current_env = getenv(str_asciz(name));

  arr->arr = xrealloc(arr->arr, ++arr->size * sizeof(struct env));

  entry = &arr->arr[arr->size-1];
  env_ctor(entry);

  str_set_str(&entry->name, name);

  if (current_env != NULL)
    {
      str_ctor(&env_buf);
      str_append_asciz(&env_buf, current_env);
      env_setenv(entry, &env_buf);
      str_dtor(&env_buf);
    }
  else
    entry->unset = 1;

  entry->clean = 1;

  env_sane(entry);

  return entry;
}


void
envarray_dump(struct envarray *arr,
	      FILE *f,
	      const char *sh)
{
  int i;

  for (i = 0; i < arr->size; i++)
    if (!arr->arr[i].clean)
      env_dump(&arr->arr[i], f, sh);
}


void
envarray_map(struct envarray *arr,
	     void (*cb)(struct env *))
{
  int i;

  for (i = 0; i < arr->size; i++)
    (*cb)(&arr->arr[i]);
}


struct envarray *
create_envarray(void)
{
  struct envarray *result;

  result = xmalloc(sizeof(struct envarray));
  result->size = 0;
  result->arr = NULL;
  return result;
}


void
destroy_envarray(struct envarray *arr)
{
  int i;

  for (i = 0; i < arr->size; i++)
    env_dtor(&arr->arr[i]);
  xfree(arr->arr);
  xfree(arr);
}
