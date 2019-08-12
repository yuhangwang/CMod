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

/* Note: <sys/types.h> must be included before this file
   is included.  Don't include <sys/types.h> here, since it cannot be
   included twice on some brain-damaged systems.  */

#ifndef CMOD_WAIT_H
#define CMOD_WAIT_H

#if HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#ifndef WIFEXITED
#  define WIFEXITED(s)   ((s) & 0xff == 0)
#endif

#ifndef WEXITSTATUS
#  define WEXITSTATUS(s) (((s) >> 8) & 0xff)
#endif

#ifndef WIFSIGNALED
#  define WIFSIGNALED(s) (!WIFEXITED((s)) && WEXITSTATUS((s)) == 0)
#endif

#ifndef WTERMSIG
#  define WTERMSIG(s)	 ((s) & 0x7f)
#endif

#endif
