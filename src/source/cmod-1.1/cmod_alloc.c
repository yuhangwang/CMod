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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "cmod_alloc.h"
#include "cmod_main.h"

char cmod_alloc_null_object[] = "";

static void
out_of_memory(void)
{
  fprintf(stderr, "%s: ", argv0);
  perror("while allocating memory");
  exit(CMOD_EXIT_ERROR);
}


void *
xmalloc(size_t size)
{
  if (size == 0)
    return &cmod_alloc_null_object;
  else
    {
      void *buf = malloc(size);
      if (buf == NULL)
	out_of_memory();
      return buf;
    }
}
  
void
xfree(void *ptr)
{
  if (ptr != NULL && ptr != &cmod_alloc_null_object)
    free(ptr);
}

void *
xrealloc(void *ptr, size_t size)
{
  if (ptr == NULL || ptr == &cmod_alloc_null_object)
    return xmalloc(size);
  else if (size == 0)
    {
      xfree(ptr);
      return &cmod_alloc_null_object;
    }
  else
    {
      void *buf = realloc(ptr, size);
      if (buf == NULL)
	out_of_memory();
      return buf;
    }
}

char *
xstrdup(const char *s)
{
  char *copy = strdup(s);
  if (copy == NULL)
    out_of_memory();
  return copy;
}
