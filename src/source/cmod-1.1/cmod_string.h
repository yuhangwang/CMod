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

/* String operations */

#ifndef CMOD_STRING_H
#define CMOD_STRING_H

/* A string.  Nothing outside cmod_string.c should access the data of
   the struct directly, since the representation of a string may
   change at any time. */
struct cmod_str_internal;

struct cmod_str {
  struct cmod_str_internal *si;
};

#ifdef PROTOTYPES

/* Include all prototypes. */
#  include "cmod_strp.h"

#else

/* Prototype-less variant of cmod_strp.h.  This is generated from
   cmod_strn.h.  */
#  include "cmod_strn.h"

#endif

#endif
