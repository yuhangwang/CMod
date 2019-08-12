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

/* Parse module files */

#ifndef CMOD_PARSE_H
#define CMOD_PARSE_H

struct cmd {
  /* The words of the command.  cmd2 is NULL for single-word commands.
     cmd1 is NULL to mark the end of the table of "struct cmd" entries. */
  const char *cmd1;
  const char *cmd2;

  /* Number of required arguments. */
  int min_args;

  /* -1 for unlimited. */
  int max_args;

  /* Callback functions.  They should return 0 for success.  Any other
     return value will cause the module that contains the command to
     be aborted.  When a module is aborted all commands prior to the
     erring command will be undone.  The command that fails should not
     alter the environment (or undo its own alterations).  */
#ifdef PROTOTYPES
  int (*doit)(const struct cmod_str *from_module, const int *from_line,
	      int argc, const struct cmod_str *argv);
  int (*undo)(const struct cmod_str *from_module, const int *from_line,
	      int argc, const struct cmod_str *argv);
#else
  int (*doit)();
  int (*undo)();
#endif
};

#ifdef PROTOTYPES

/* Return value:
    0 ok
    1 syntax error */
extern int cmod_parse_module(FILE *fp,
			     const struct cmod_str *filename,
			     int for_removal,
			     const struct cmd *parse_table,
			     int *lineno);

extern int cmod_parse_argv(const struct cmd *parse_table,
			   int argc,
			   const char **argv);
#else
extern int cmod_parse_module();
extern int cmod_parse_argv();
#endif

#endif
