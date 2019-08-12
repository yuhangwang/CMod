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

/* This file acts as a wrapper around /bin/sh, removing everything from
   the environmnt, except for the variable binding passed as argv[1].

   This is used for running tests in the following way:

       ./clean-sh MODULEPATH=$(srcdir) testscript                   */

#include <config.h>

#include <sys/types.h>
#include <unistd.h>

#include "unused.h"

int
main(int UNUSED(argc), const char *argv[])
{
  const char *envp[3];
  envp[0] = argv[1];
  envp[1] = "PATH=";
  envp[2] = 0;
  argv[1] = "/bin/sh";
  execve("/bin/sh", argv+1, envp);
  return 1;
}
