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

#include <config.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include "archaic.h"
#include "cmod_alloc.h"
#include "cmod_string.h"

/* BUFFER points to a dynamically allocated buffer which is SIZE bytes large.
   The buffer is typically larger than the actual string.  There may
   be unoccupied bytes before and/or after the string.  The string is
   always followed by a NUL byte.

   START points to the first byte in the string.  END points to the
   NUL byte that follows the actual string.  (This NUL byte does not
   conceptually belong to the string.  It is present there to simplify
   the API and implementation of str_asciz().)  START and END always
   point within the buffer.  The length of the string can be
   calculated as END-START.
   
   The empty string has two basic representations:
     START, END, BUFFER and SIZE may all be 0.
     BUFFER and SIZE may specify a buffer of any size (from 1 and up),
     and START and END will point to the NUL-byte within that buffer. */

struct cmod_str_internal {
  int  refcnt;
  long size;
  char *buffer;
  char *start;
  char *end;
};

#ifdef PROTOTYPES
static void str_own(struct cmod_str *s);
#else
static void str_own();
#endif

#ifdef NDEBUG
#define str_sane(x) do { } while (0)
#else

static void
str_sane(const struct cmod_str *s)
{
  assert(s->si != NULL);
  assert(s->si->refcnt > 0);
  assert(s->si->size >= 0);
  if (s->si->size > 0)
    {
      assert(s->si->buffer != NULL);
      assert(s->si->buffer != CMOD_NULL);
      assert(s->si->start != NULL);
      assert(s->si->end != NULL);
      assert(s->si->start >= s->si->buffer);
      assert(s->si->end >= s->si->buffer);
      assert(s->si->start < s->si->buffer + s->si->size);
      assert(s->si->end < s->si->buffer + s->si->size);
      assert(s->si->start <= s->si->end);
      assert(s->si->end[0] == '\0');
    }
  else
    {
      assert(s->si->buffer == NULL);
      assert(s->si->start == NULL);
      assert(s->si->end == NULL);
    }
}

#endif

static void
require(struct cmod_str *s,
	long len)
{
  long wanted;
  char *new_buf;

  str_sane(s);

  if (len == 0)
    /* No need to allocate. */
    return;

  if (s->si->buffer != NULL
      && s->si->end - s->si->buffer + len + 1 <= s->si->size)

    /* There is room left after end. */
    return;

  if (s->si->buffer != NULL
      && s->si->end - s->si->start + len + 1 <= s->si->size)
    {
      assert(s->si->start > s->si->buffer);

      /* There is room if we move the data to the start of the buffer.
         However, we want to avoid moving a large block a small
	 distance repeatedly; reallocate the block anyhow if the
	 condition on the next line returns false. */

      if (4 * (s->si->start - s->si->buffer) >= s->si->size)
	{
	  memmove(s->si->buffer, s->si->start, s->si->end - s->si->start);
	  s->si->end -= s->si->start - s->si->buffer;
	  s->si->start = s->si->buffer;
	  return;
	}
    }

  if (s->si->buffer != NULL)
    {
      /* Reallocate the buffer to get some space. */
      long wanted_min = s->si->end - s->si->start + len + 1;
      long wanted_nomove = s->si->end - s->si->buffer + len + 1;
      long wanted_double = s->si->size + s->si->size;

      assert(wanted_nomove >= wanted_min);

      wanted = wanted_double;
      if (wanted_min > wanted)
	wanted = wanted_min;

      if (wanted_nomove < wanted + wanted)
	wanted = wanted_nomove;
    }
  else
    wanted = len + 1;

  if (wanted < 8)
    wanted = 8;

  new_buf = xrealloc(s->si->buffer, wanted);
  s->si->size = wanted;
  if (s->si->buffer != NULL)
    {
      s->si->start = s->si->start - s->si->buffer + new_buf;
      s->si->end = s->si->end - s->si->buffer + new_buf;
      s->si->buffer = new_buf;
    }
  else
    {
      s->si->start = s->si->end = s->si->buffer = new_buf;
      *s->si->end = '\0';
    }

  assert(s->si->buffer != NULL);

  if (s->si->end - s->si->buffer + len + 1 <= s->si->size)
    /* There is room left after end. */
    return;

  /* Move data to the start of the block. */
  assert(s->si->end - s->si->start + len + 1 <= s->si->size);
  memmove(s->si->buffer, s->si->start, s->si->end - s->si->start);
  s->si->end -= s->si->start - s->si->buffer;
  s->si->start = s->si->buffer;
  return;
}

void
str_ctor(struct cmod_str *s)
{
  s->si = xmalloc(sizeof(struct cmod_str_internal));
  s->si->refcnt = 1;
  s->si->size = 0;
  s->si->buffer = NULL;
  s->si->start = NULL;
  s->si->end = NULL;
  str_sane(s);
}

void
str_dtor(struct cmod_str *s)
{
  str_sane(s);
  assert(s->si != NULL);
  assert(s->si->refcnt > 0);
  if (s->si->refcnt-- == 1)
    {
      xfree(s->si->buffer);
      xfree(s->si);
    }
}

void
str_clear(struct cmod_str *s)
{
  str_own(s);
  s->si->start = s->si->end = s->si->buffer;
  if (s->si->size != 0)
    *s->si->buffer = '\0';

  str_sane(s);
}

void
str_append_asciz(struct cmod_str *s,
		 const char *cstr)
{
  long len;

  str_sane(s);

  len = strlen(cstr);
  if (len > 0)
    {
      str_own(s);
      require(s, len);
      memcpy(s->si->end, cstr, len);
      s->si->end += len;
      *s->si->end = '\0';
    }
  str_sane(s);
}

void
str_append_str(struct cmod_str *dest,
	       const struct cmod_str *src)
{
  str_sane(dest);
  str_sane(src);
  if (src->si->end != src->si->start)
    {
      long len = src->si->end - src->si->start;
      str_own(dest);
      require(dest, len);
      memcpy(dest->si->end, src->si->start, len);
      dest->si->end += len;
      *dest->si->end = '\0';
    }
  str_sane(dest);
  str_sane(src);
}

void
str_append_buf(struct cmod_str *s,
	       const char *block,
	       long len)
{
  str_sane(s);
  if (len > 0)
    {
      str_own(s);
      require(s, len);
      memcpy(s->si->end, block, len);
      s->si->end += len;
      *s->si->end = '\0';
    }
  str_sane(s);
}

void
str_append_char(struct cmod_str *s,
		int c)
{
  /* This function is careful to accept a "char" that has been
     promoted to an "int" regardless of if "char" is a signed or
     unsigned type.  It is illegal to pass EOF to this function, but
     it will not detect if anybody is stupid enough to do so.  */
  char ch = (char)c;

  str_own(s);
  str_append_buf(s, &ch, 1);
}


void
str_set_str(struct cmod_str *copy,
	    const struct cmod_str *orig)
{
  str_clear(copy);
  str_append_str(copy, orig);
}

void
str_truncate(struct cmod_str *s,
	     long len)
{
  str_own(s);

  assert(s != NULL);
  assert(s->si != NULL);
  assert(s->si->refcnt > 0);
  assert(s->si->start != NULL);
  assert(s->si->end != NULL);

  assert(len <= s->si->end - s->si->start);

  s->si->end = s->si->start + len;
  *s->si->end = '\0';

  str_sane(s);
}

void
str_remove_first(struct cmod_str *s,
		 long len)
{
  str_own(s);

  assert(s != NULL);
  assert(s->si != NULL);
  assert(s->si->refcnt > 0);
  assert(s->si->start != NULL);
  assert(s->si->end != NULL);

  assert(len <= s->si->end - s->si->start);

  s->si->start += len;

  str_sane(s);
}


void
str_remove_last(struct cmod_str *s,
		long len)
{
  str_own(s);

  assert(s != NULL);
  assert(s->si != NULL);
  assert(s->si->refcnt > 0);
  assert(s->si->start != NULL);
  assert(s->si->end != NULL);

  assert(len <= s->si->end - s->si->start);

  s->si->end -= len;
  *s->si->end = '\0';

  str_sane(s);
}

const char *
str_asciz(const struct cmod_str *s)
{
  static char empty[] = "";

  str_sane(s);
  if (s->si->start == NULL)
    {
      assert(s->si->size == 0);
      assert(s->si->buffer == NULL);
      return empty;
    }
  return s->si->start;
}

long
str_strlen(const struct cmod_str *s)
{
  str_sane(s);
  if (s->si->start == NULL)
    return 0;
  else
    return s->si->end - s->si->start;
}

int
str_empty(const struct cmod_str *s)
{
  str_sane(s);
  return s->si->start == NULL || s->si->end == s->si->start;
}

long
str_strchr(const struct cmod_str *s,
	   char c)
{
  char *hay;

  if (str_empty(s))
    return -1;

  for (hay = s->si->start; hay != s->si->end; ++hay)
    if (*hay == c)
      return hay - s->si->start;

  return -1;
}


int
str_strcmp(const struct cmod_str *a,
	   const struct cmod_str *b)
{
  str_sane(a);
  str_sane(b);
  if (a->si->start == NULL)
    if (b->si->start == NULL)
      return 0;
    else
      return -(unsigned char)b->si->start[0];
  else
    if (b->si->start == NULL)
      return (unsigned char)a->si->start[0];
    else
      return strcmp(a->si->start, b->si->start);
}


int
str_tail(const struct cmod_str *s)
{
  str_sane(s);
  if (s->si->start == s->si->end)
    return -1;
  else
    return (unsigned char)*(s->si->end - 1);
}


void
str_copy(struct cmod_str *copy,
	 const struct cmod_str *orig)
{
  str_sane(orig);
  copy->si = orig->si;
  copy->si->refcnt++;
  str_sane(copy);
  str_sane(orig);
}

static void
str_own(struct cmod_str *s)
{
  str_sane(s);
  if (s->si->refcnt > 1)
    {
      struct cmod_str copy;
      str_ctor(&copy);
      str_append_str(&copy, s);
      s->si->refcnt--;
      s->si = copy.si;
    }
  str_sane(s);
}

struct cmod_str *
copy_str_array(int siz, struct cmod_str *arr)
{
  struct cmod_str *res = xmalloc(siz * sizeof(struct cmod_str));
  int i;

  for (i = 0; i < siz; ++i)
    str_copy(&res[i], &arr[i]);

  return res;
}

void
free_str_array(int siz, struct cmod_str *arr)
{
  int i;

  for (i = 0; i < siz; ++i)
    str_dtor(&arr[i]);

  xfree(arr);
}
