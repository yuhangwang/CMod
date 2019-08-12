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

/* Note: <sys/types.h> must be included before this file
   is included.  Don't include <sys/types.h> here, since it cannot be
   included twice on some brain-damaged systems.  */

#ifndef CMOD_ALLOC_H
#define CMOD_ALLOC_H

#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif

/* Allocator functions.  These functions never return unless they
   succeed in allocating the required memory.  Allocating a zero-sized
   object returns a non-NULL pointer.  (Actually, all zero-sized
   objects return the same pointer, so xmalloc(0)==xmalloc(0) is
   always true. */

#ifdef PROTOTYPES
void *xmalloc(size_t size);
void xfree(void *ptr);
void *xrealloc(void *ptr, size_t size);
char *xstrdup(const char *s);
#else
void *xmalloc();
void xfree();
void *xrealloc();
char *xstrdup();
#endif

/* The cmod_alloc_null_object and CMOD_NULL should only be used for
   debugging purposes. */
extern char cmod_alloc_null_object[];
#define CMOD_NULL ((void*)&cmod_alloc_null_object)

#endif
