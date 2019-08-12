/* cmod - naive memmove fallback function.
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

#include <sys/types.h>

void *
memmove(dst, src, n)
     void *dst;
     const void *src;
     size_t n;
{
  char *d = dst;
  const char *s = src;

  if (d == s || n == 0)
    return dst;
  else if (s < d && d < s + n)
    {
      d += n;
      s += n;
      while (n-- > 0)
	*--d = *--s;
      return dst;
    }
  else
    {
      while (n-- > 0)
	*d++ = *s++;
      return dst;
    }
}
