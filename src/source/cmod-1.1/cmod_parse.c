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
#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>

#include "cmod_string.h"
#include "cmod_parse.h"
#include "cmod_main.h"
#include "cmod_alloc.h"
#include "env.h"
#include "archaic.h"
#include "cmod_token.h"

struct undo_chain;

struct undo_chain {
#ifdef PROTOTYPES
  int (*undo)(const struct cmod_str *filename, const int *line,
	      int argc, const struct cmod_str *argv);
#else
  int (*undo)();
#endif
  
  int from_line;
  int argc;
  struct cmod_str *argv;
  struct undo_chain *prev;
};


static const struct cmd *
locate_match(int argc,
	     const struct cmod_str *argv,
	     const struct cmd *parse_table)
{
  const struct cmd *attempt;

  if (argc < 1)
    return NULL;

  for (attempt = parse_table; attempt->cmd1 != NULL; ++attempt)
    {
      /* Does the first word match? */
      if (strcmp(str_asciz(&argv[0]), attempt->cmd1))
	continue;

      /* Must the second word match as well? */
      if (attempt->cmd2 != NULL)
	{
	  /* Yes, the second word must match. */
	  /* (There is no way to have options before the first command.) */
	  if (argc < 2)
	    return NULL;
	  if (strcmp(str_asciz(&argv[1]), attempt->cmd2))
	    continue;
	}

      /* We have found a match.  */
      return attempt;
    }

  return NULL;
}


static void
syntax_error(const char *errmsg,
	     const struct cmod_str *filename,
	     const int *lineno,
	     int argc,
	     struct cmod_str *argv)
{
  int i;

  fprintf(stderr, "%s:%d: syntax error: %s\n",
	  str_asciz(filename), *lineno, errmsg);
  fprintf(stderr, "  Offending line: ");
  for (i = 0; i < argc; ++i)
    {
      if (i)
	putc(' ', stderr);
      fprintf(stderr, "%s", str_asciz(&argv[i]));
    }
  putc('\n', stderr);
}

int
cmod_parse_module(FILE *fp,
		  const struct cmod_str *filename,
		  int for_removal,
		  const struct cmd *parse_table,
		  int *lineno)
{
  struct cmod_str *argv = NULL;
  int argc = 0;
  int arg_alloc = 0;
  int adjust;
  struct undo_chain *undo = NULL;
  const struct cmd *match;
  int err_found = 0;
  struct undo_chain *head;
  struct cmod_str tokenizer_buf;
  int eof_seen = 0;
  enum cmod_status token_stat;

  str_ctor(&tokenizer_buf);
  
  while ((token_stat = cmod_tokenize(fp, filename, &tokenizer_buf, &eof_seen,
				     lineno, &argc, &argv, &arg_alloc))
	 == cmod_ok)
    {
      match = locate_match(argc, argv, parse_table);

      if (match == NULL)
	{
	  syntax_error("unknown command", filename, lineno, argc, argv);
	  err_found = 1;
	  break;
	}

      adjust = (match->cmd1 != NULL) + (match->cmd2 != NULL);
      
      /* Check argument count. */
      if (argc - adjust < match->min_args)
	{
	  syntax_error("too few arguments", filename, lineno, argc, argv);
	  err_found = 1;
	  break;
	}

      if (match->max_args != -1 && argc - adjust > match->max_args)
	{
	  syntax_error("too many arguments", filename, lineno, argc, argv);
	  err_found = 1;
	  break;
	}
	    
      /* We have the arguments.  Perform the command. */
      if ((*match->doit)(filename, lineno, argc - adjust, argv + adjust) != 0)
	{
	  /* We expect the command to print the error message.  */
	  err_found = 1;
	  break;
	}

      /* Put this command on the undo chain. */
      head = xmalloc(sizeof(struct undo_chain));
      head->undo = match->undo;
      head->argc = argc - adjust;
      head->argv = copy_str_array(argc - adjust, argv + adjust);
      head->from_line = *lineno;
      head->prev = undo;
      undo = head;
    }

  if (token_stat == cmod_abort_module)
    err_found = 1;

  str_dtor(&tokenizer_buf);

  while (undo != NULL)
    {
      struct undo_chain *prev = undo->prev;
      if (for_removal || err_found)
	(*undo->undo)(filename, &undo->from_line, undo->argc, undo->argv);
      free_str_array(undo->argc, undo->argv);
      xfree(undo);
      undo = prev;
    }

  free_str_array(arg_alloc, argv);

  return err_found;
}

int
cmod_parse_argv(const struct cmd *parse_table,
		int argc,
		const char **argv)
{
  const struct cmd *match;
  struct cmod_str *argv_str;
  int i;
  int res;

  if (argc < 1)
    {
      fprintf(stderr, "missing subcommand\n");
      return -1;
    }

  argv_str = xmalloc((argc + 1) * sizeof(struct cmod_str));
  str_ctor(&argv_str[0]);
  str_append_asciz(&argv_str[0], "module");
  for (i = 0; i < argc; ++i)
    {
      str_ctor(&argv_str[i+1]);
      str_append_asciz(&argv_str[i+1], argv[i]);
    }

  match = locate_match(argc + 1, argv_str, parse_table);
  res = 0;
  if (match == NULL)
    res = -1;

  if (res == 0 && argc - 1 < match->min_args) 
    {
      fprintf(stderr, "missing argument to %s%s%s\n", match->cmd1,
	      match->cmd2 ? " ": "", match->cmd2 ? match->cmd2: "");
      res = -1;
    }

  if (res == 0 && match->max_args != -1 && argc - 1 > match->max_args)
    {
      fprintf(stderr, "too many arguments to %s%s%s\n", match->cmd1,
	      match->cmd2 ? " ": "", match->cmd2 ? match->cmd2: "");
      res = -1;
    }

  if (res == 0)
    res = (*match->doit)(NULL, NULL, argc - 1, argv_str + 2);

  free_str_array(argc + 1, argv_str);
  return res;
}

/* cmod_parse.c ends here */
