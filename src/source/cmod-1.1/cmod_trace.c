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
#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif
#include <sys/types.h>

#include "cmod_trace.h"

static int mask = 0;

void
set_traces(int traces)
{
  mask = traces;
}

#if defined(HAVE_VPRINTF) && defined(PROTOTYPES) && defined(HAVE_STDARG_H)
void
trace(int tr_mask, const char *fmt, ...)
{
  va_list v;
  if (tr_mask & mask)
    {
      va_start(v, fmt);
      vfprintf(stderr, fmt, v);
      va_end(v);
    }
}
#else
void
trace(int tr_mask, const char *fmt,
      int a, int b, int c, int d, int e, int f, int g)
{
  if (tr_mask & mask)
    fprintf(stderr, fmt, a, b, c, d, e, f, g);
}
#endif
