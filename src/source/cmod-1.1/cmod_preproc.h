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

#ifndef CMOD_PREPROCESS_H
#define CMOD_PREPROCESS_H

#ifdef PROTOTYPES

extern enum cmod_status
cmod_preprocess(FILE *file,                      /* in */
		const struct cmod_str *filename, /* in */
		int end_char,			 /* in */
		int *eof_seen,                   /* in/out */
		int *lineno,                     /* in/out */
		struct cmod_str *res);           /* append */

#else

extern enum cmod_status cmod_preprocess();

#endif
#endif
