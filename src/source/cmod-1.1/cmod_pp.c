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

#include <config.h>

#include <assert.h>
#include <stdio.h>
#include <sys/types.h>

#include "cmod_string.h"
#include "cmod_token.h"
#include "cmod_preproc.h"
#include "archaic.h"
#include "cmod_main.h"
#include "env.h"
#include "cmod_process.h"
#include "cmod_wait.h"


#ifdef PROTOTYPES

static enum cmod_status
dollar(FILE *file,
       const struct cmod_str *filename,
       int *eof_seen,
       int *lineno,
       struct cmod_str *res);

#else

static enum cmod_status dollar();

#endif


static void
prep_error(const char *errmsg,
	   const struct cmod_str *filename,
	   int *lineno)
{
  fprintf(stderr, "%s:%d:%s",
	  str_asciz(filename), 1 + *lineno, errmsg);
}

/* Variable lookup.  We support these modifiers:

    ${x}    -- use value of y; fail if unset 
    ${x:-y} -- use y if x is not set
    ${x:=y} -- assign y to x if x not set
    ${x:?y} -- print y and fail if x not set
    ${x:+y} -- use y if x is set, "" if not

   The colon may, as usual, be omitted (see bash(1) for more info). */

static_inline enum cmod_status
variable(FILE *file,                      /* in */
	 const struct cmod_str *filename, /* in */
	 int *eof_seen,		          /* in/out */
	 int *lineno,                     /* in/out */
	 struct cmod_str *res)	          /* append */
{
  int unset;
  int colon_seen = 0;
  struct cmod_str name;
  struct cmod_str arg;
  struct cmod_str *argp = NULL;
  struct cmod_str value;
  int modifier = 0;
  int c;
  int done = 0;
  struct env *env_var;
  enum cmod_status ret;

  str_ctor(&name);

  while (!done)
    {
      if ((c = getc(file)) == EOF)
	{
	  prep_error("unexpected end of file in variable reference\n",
		     filename, lineno);
	  *eof_seen = 1;
	  str_dtor(&name);
	  return cmod_abort_module;
	}

      switch(c)
	{
	case '}':
	  done = 1;		/* Got everything. */
	  break;
	case '\n':
	  prep_error("unterminated variable reference\n", filename, lineno);
	  str_dtor(&name);
	  return cmod_abort_module;
	case ':':
	  if (colon_seen)
	    {
	      prep_error("multiple colons in variable expansion\n",
			 filename, lineno);
	      str_dtor(&name);
	      return cmod_abort_module;
	    }
	  colon_seen = 1;
	  break;
	case '-':
	case '=':
	case '?':
	case '+':
	  modifier = c;
	  done = 2;		/* Still need to parse the argument. */
	  break;
	default:
	  if (colon_seen)
	    {
	      prep_error("expected one of '-=+?' after colon\n",
			 filename, lineno);
	      str_dtor(&name);
	      return cmod_abort_module;
	    }
	  str_append_char(&name, c);
	  break;
	}
    }

  if (res == NULL)
    {
      /* Don't bother looking up the variable if the value is unused.
	 However, we may have to parse past the argument. */
      str_dtor(&name);
      if (done == 2)
	return cmod_preprocess(file, filename, '}', eof_seen, lineno, NULL);
      else
	return cmod_ok;
    }

  env_var = envarray_locate_create(global_env, &name);
  str_ctor(&value);
  switch (env_value(env_var, &value))
    {
    case 0:		/* normal */
    case 1:		/* clean */
      if (colon_seen && modifier)
	unset = str_empty(&value);
      else
	unset = 0;
      break;
    case 2:		/* unset */
    case 3:		/* unset+clean */
      unset = 1;
      break;
    default:
      assert(0);
    }

  if (done == 2)
    {
      assert(modifier);

      /* We need to parse the argument to the modifier.  However, we
	 only need to process it in some cases.  A careful examination
	 of the code below reveals that "unset XOR modifier == '+'"
	 determines if we need it or not.  Below this point we use
	 argp to reach the argument when it is set.  */

      if (unset ^ (modifier == '+'))
	{
	  argp = &arg;
	  str_ctor(argp);
	}
      ret = cmod_preprocess(file, filename, '}', eof_seen, lineno, argp);
      if (ret != cmod_ok)
	{
	  str_dtor(&name);
	  if (argp)
	    str_dtor(argp);
	  return ret;
	}
    }
  else
    assert(!modifier);
  
  if (unset && ((modifier == '?' && str_empty(argp)) || !modifier))
    {
      fprintf(stderr, "%s:%d:unset variable %s\n",
	      str_asciz(filename), 1 + *lineno,
	      str_asciz(&name));
      str_dtor(&name);
      if (argp)
	str_dtor(argp);
      str_dtor(&value);
      return cmod_abort_module;
    }
  if (unset)
    switch (modifier)
      {
      case '=':
	env_setenv(env_var, argp);
	/* FALLTHROUGH */
      case '-':
	str_set_str(&value, argp);
	break;
      case '?':
	fprintf(stderr, "%s\n", str_asciz(argp));
	str_dtor(&name);
	str_dtor(argp);
	str_dtor(&value);
	return cmod_abort_module;
      case '+':
	str_clear(&value);
	break;
      default:
	assert(0);
      }
  else
    if (modifier == '+')
      str_set_str(&value, argp);

  if (res != NULL)
    str_append_str(res, &value);

  if (argp)
    str_dtor(argp);
  str_dtor(&name);
  str_dtor(&value);
  
  return cmod_ok;
}

static_inline enum cmod_status
script_backslash(FILE *file,                      /* in */
		 const struct cmod_str *filename, /* in */
		 int *eof_seen,		          /* in/out */
		 int *lineno,                     /* in/out */
		 struct cmod_str *res)	          /* append */
{
  int c;

  if ((c = getc(file)) == EOF)
    {
      prep_error("unexpected end of file after backslash\n", filename, lineno);
      *eof_seen = 1;
      return cmod_abort_module;
    }

  switch(c)
    {
    case '$':
    case '(':
    case '\\':
    case ')':
      if (res != NULL)
	str_append_char(res, c);
      return cmod_ok;
    case '\n':
      ++*lineno;
      return cmod_ok;
    default:
      prep_error("bad backslash escape in shell escape\n", filename, lineno);
      return cmod_abort_module;
    }
}


static_inline enum cmod_status
script(FILE *file,                      /* in */
       const struct cmod_str *filename, /* in */
       int *eof_seen,		        /* in/out */
       int *lineno,                     /* in/out */
       struct cmod_str *res)	        /* append */
{
  struct cmod_str cmd;
  struct cmod_str cmd_stdout;
  int done = 0;
  enum cmod_status ret;
  int exit_status;
  int c;

  str_ctor(&cmd);

  while (!done)
    {
      if ((c = getc(file)) == EOF)
	{
	  prep_error("unexpected end of file in shell escape\n",
		     filename, lineno);
	  *eof_seen = 1;
	  str_dtor(&cmd);
	  return cmod_abort_module;
	}

      if (c == '\n')
	++*lineno;

      switch (c)
	{
	case '\\':
	  ret = script_backslash(file, filename, eof_seen, lineno, &cmd);
	  if (ret != cmod_ok)
	    {
	      str_dtor(&cmd);
	      return ret;
	    }
	  break;
	case '$':
	  ret = dollar(file, filename, eof_seen, lineno, &cmd);
	  if (ret != cmod_ok)
	    {
	      str_dtor(&cmd);
	      return ret;
	    }
	  break;
	case ')':
	  done = 1;
	  break;
	default:
	  str_append_char(&cmd, c);
	  break;
	}
    }

  if (res == NULL)
    {
      /* Don't even run the command if the output isn't used.  */
      str_dtor(&cmd);
      return cmod_ok;
    }

  str_ctor(&cmd_stdout);
  exit_status = cmod_popen(&cmd, &cmd_stdout);
  if (WIFEXITED(exit_status) && WEXITSTATUS(exit_status) != 0)
    {
      prep_error("command failed with exit status ", filename, lineno);
      fprintf(stderr, "%d\n", WEXITSTATUS(exit_status));
      str_dtor(&cmd_stdout);
      str_dtor(&cmd);
      return cmod_abort_module;
    }
  else if (WIFSIGNALED(exit_status))
    {
      prep_error("command exited due to signal ", filename, lineno);
      fprintf(stderr, "%d", WTERMSIG(exit_status));
#ifdef WCOREDUMP
      if (WCOREDUMP(exit_status))
	fputs(" (core dumped)", stderr);
#endif
      putc('\n', stderr);
      str_dtor(&cmd_stdout);
      str_dtor(&cmd);
      return cmod_abort_module;
    }

  /* Remove a trailing newline, if present. */
  if (str_tail(&cmd_stdout) == '\n')
    str_remove_last(&cmd_stdout, 1);

  str_append_str(res, &cmd_stdout);
  str_dtor(&cmd_stdout);
  str_dtor(&cmd);
  return cmod_ok;
}


static enum cmod_status
dollar(FILE *file,                      /* in */
       const struct cmod_str *filename, /* in */
       int *eof_seen,		        /* in/out */
       int *lineno,                     /* in/out */
       struct cmod_str *res)		/* append */
{
  int c;

  if ((c = getc(file)) == EOF)
    {
      prep_error("unexpected EOF after dollar\n", filename, lineno);
      *eof_seen = 1;
      return cmod_abort_module;
    }

  switch(c)
    {
    case '{':
      return variable(file, filename, eof_seen, lineno, res);
    case '(':
      return script(file, filename, eof_seen, lineno, res);
    case '$':
      if (res != NULL)
	str_append_char(res, '$');
      return cmod_ok;
    case '\n':
      prep_error("unexpected newline after dollar\n", filename, lineno);
      return cmod_abort_module;
    default:
      prep_error("bad dollar substitution\n", filename, lineno);
      return cmod_abort_module;
    }
}


enum cmod_status
cmod_preprocess(FILE *file,                      /* in */
		const struct cmod_str *filename, /* in */
		int end_char,			 /* in */
		int *eof_seen,                   /* in/out */
		int *lineno,                     /* in/out */
		struct cmod_str *res) 		 /* append */
{
  /* FIXME: ugly handling of end_char below...  */
  int c;
  enum cmod_status ret;

  assert(end_char != '\n' || res != NULL);

  if (*eof_seen)
    return cmod_eof;

  while ((c = getc(file)) != EOF)
    {
      if (c == '$')
	{
	  /* FIXME: should remember the lenght of RES here and only use
	     str_strchr on the new part when searching for the newline. */

	  if ((ret = dollar(file, filename, eof_seen, lineno, res)) != cmod_ok)
	    return ret;
	  if (end_char == '\n' && str_strchr(res, end_char) != -1)
	    return cmod_ok;
	}
      else
	{
	  if (c == '\n')
	    ++*lineno;
	  if (res != NULL && (end_char == '\n' || c != end_char))
	    str_append_char(res, c);
	  if (c == end_char)
	    return cmod_ok;
	}
    }

  *eof_seen = 1;
  return (str_empty(res) || res == NULL) ? cmod_eof : cmod_ok;
}
