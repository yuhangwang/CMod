/* cmod - set up the user environment with module files.
   Copyright (C) 1998 by the authors of cmod (see AUTHORS).

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

/* Handle subprocesses. */

#include <config.h>

#include <sys/types.h>
#include <assert.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#  include <stdlib.h>	/* for proper declaration of environ */
#endif
#include <string.h>

/* At least under Solaris 2.6, we need to declare environ ourself.
   This is probably the most portable solution.  Why?  Därför att
   Emacs gör så! */
extern char **environ;

#include "cmod_string.h"
#include "cmod_process.h"
#include "cmod_main.h"
#include "cmod_alloc.h"
#include "env.h"
#include "archaic.h"

static const char **child_environ = NULL;
static int child_env_size = 0;
static int child_next = 0;


static void
grow_environ(void)
{
  if (child_env_size < child_next + 1)
    {
      child_env_size = 2 * child_env_size;
      if (child_env_size < child_next + 4)
	child_env_size = child_next + 4;

      child_environ = xrealloc(child_environ, child_env_size * sizeof(char*));
    }
}


static void
append_env(const char *envp)
{
  grow_environ();
  child_environ[child_next++] = envp;
}

#ifdef NDEBUG
#define assert_not_match(a, b) do { } while(0)
#else

static void
assert_not_match(const char *a,
		 const char *b)
{
  assert(a != NULL);
  assert(*a != '\0');
  assert(*a != '=');

  assert(b != NULL);
  assert(*b != '\0');
  assert(*b != '=');

  while (*a != '=' && *b != '=')
    {
      if (*a != *b)
	return;

      ++a;
      ++b;
      assert(*a != '\0');
      assert(*b != '\0');
    }

  assert(*a != *b);
}

#endif /* NDEBUG */

static void
add_if_not_present(struct env *e)
{
  const char *env;
  if (env_environ(e, &env) == 0)
    {
      int ix;

      for (ix = 0; ix < child_next; ++ix)
	{
	  /* The very same string? */
	  if (env == child_environ[ix])
	    return;

	  /* We should have no match here, unless we got the very same
	     string.  This relies on the fact that env_environ
	     returned 0.  Clean strings are taken from the real
	     environ, not from global_env. */
	  assert_not_match(env, child_environ[ix]);
	}

      /* Found no match, so add this. */
      append_env(env);
    }
}


static void
setup_child_environ(void)
{
  char **envp;
  struct cmod_str varname;
  const char *varvalue;
  struct env *e;

  child_next = 0;

  str_ctor(&varname);

  for (envp = environ; *envp; ++envp)
    {
      char *eq = strchr(*envp, '=');
      if (eq == NULL)
	{
	  fprintf(stderr, "%s: broken environment '%s'\n", argv0, *envp);
	  exit(CMOD_EXIT_ERROR);
	}
	
      str_clear(&varname);
      str_append_buf(&varname, *envp, eq - *envp);

      e = envarray_locate(global_env, &varname);
      if (e != NULL)
	{
	  switch(env_environ(e, &varvalue))
	    {
	    case 0:		/* normal */
	      append_env(varvalue);
	      break;
	    case 1:		/* clean */
	      append_env(*envp);
	      break;
	    case 2:		/* unset */
	    case 3:		/* unset+clean */
	      break;
	    default:
	      assert(0);
	    }
	}
      else
	append_env(*envp);
    }

  str_dtor(&varname);

  envarray_map(global_env, &add_if_not_present);

  append_env(NULL);
}

int
cmod_popen(const struct cmod_str *cmd,
	   struct cmod_str *res)
{
  int c;
  char **env_save;
  FILE *fp;

  setup_child_environ();

  env_save = environ;
  environ = (char**)child_environ;
  fp = popen(str_asciz(cmd), "r");
  environ = env_save;

  if (fp == NULL)
    {
      fprintf(stderr, "%s", argv0);
      perror(str_asciz(cmd));
      return -1;
    }
  while ((c = getc(fp)) != EOF)
    str_append_char(res, c);
  return pclose(fp);
}
