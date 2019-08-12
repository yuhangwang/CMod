/* cmod - not-so-naive getcwd fallback function.
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
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#include "cmod_string.h"
#include "cmod_process.h"
#include "archaic.h"

char *
getcwd(buf, size)
     char *buf;
     size_t size;
{
  struct cmod_str res;
  struct cmod_str cmd;
  int exit_status;

  if (buf == NULL || size <= 0)
    {
      errno = EINVAL;
      return NULL;
    }

  str_ctor(&res);
  str_ctor(&cmd);
  str_append_asciz(&cmd, PWD_PATH);
  exit_status = cmod_popen(&cmd, &res);
  str_dtor(&cmd);
  if (WIFEXITED(exit_status) && WEXITSTATUS(exit_status) != 0)
    {
      fprintf(stderr, "cmod: %s gave exit status %d\n", PWD_PATH,
	      WEXITSTATUS(exit_status));
      str_dtor(&res);
      errno = EACCES;		/* The most probable cause, I guess. */
      return NULL;
    }
  else if (WIFSIGNALED(exit_status))
    {
      fprintf(stderr, "cmod: %s got signal %d\n", PWD_PATH,
	      WTERMSIG(exit_status));
      str_dtor(&res);
      errno = EACCES;		/* The most probable cause, I guess. */
      return NULL;
    }

  if (str_tail(&res) == '\n')
    str_remove_last(&res, 1);

  if (size < str_strlen(&res) + 1)
    {
      str_dtor(&res);
      errno = ERANGE;
      return NULL;
    }

  strcpy(buf, str_asciz(&res));
  str_dtor(&res);
  return buf;
}
