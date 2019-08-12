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

/* A naive standalone strdup implementation for systems which lack it. */

#include <config.h>

#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif

char *strdup(s)
     char *s;
{
  char *copy = malloc(strlen(s) + 1);
  char *c;
  if (copy == 0)
    return 0;
  for (c = copy; *s != '\0'; )
    *c++ = *s++;
  *c = '\0';
  return copy;
}
