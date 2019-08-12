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

#include "cmod_alloc.h"
#include "cmod_string.h"
#include "cmod_token.h"
#include "cmod_preproc.h"
#include "archaic.h"
#include "unused.h"

enum parse_state {
  st_hunt_token,
  st_in_comment,
  st_in_token,
  st_in_str
};


enum quote_state {
  quote_none,
  quote_backslash,		/* Seen "\" */
  quote_d1,			/* Seen "\0" */
  quote_d2			/* Seen "\00" */
};


static_inline enum cmod_status
next_char(FILE *file,                      /* in */
	  const struct cmod_str *filename, /* in */
	  struct cmod_str *buffer,         /* in/out */
	  int *eof_seen,                   /* in/out */
	  int *lineno,                     /* in/out */
	  int *res)			   /* out */
{
  if (str_empty(buffer))
    {
      switch (cmod_preprocess(file, filename, '\n', eof_seen, lineno,
			      buffer))
	{
	case cmod_ok:
	  break;
	case cmod_eof:
	  return cmod_eof;
	case cmod_abort_module:
	  return cmod_abort_module;
	}
    }
  assert(!str_empty(buffer));
  
  *res = str_asciz(buffer)[0];
  str_remove_first(buffer, 1);
  return cmod_ok;
}


static void
append_token(int *used,                /* in/out */
	     struct cmod_str **tokens, /* in/out */
	     int *allocated)           /* in/out */
{
  int ix;

  ++*used;
  if (*used >= *allocated)
    {
      *tokens = xrealloc(*tokens, (*used + 1) * sizeof(struct cmod_str));
      for (ix = *allocated; ix <= *used; ++ix)
	str_ctor(&(*tokens)[ix]);
      *allocated = *used + 1;
    }
  assert(*used < *allocated);
  str_clear(&(*tokens)[*used]);
}


static void
append_char(int *used,
	    struct cmod_str **tokens,
	    int c)
{
  str_append_char(&(*tokens)[*used], c);
}


static void
tok_error(const char *errmsg,
	  const struct cmod_str *filename,
	  struct cmod_str *buffer,
	  int *lineno,
	  int *UNUSED(used_tokens),
	  struct cmod_str **UNUSED(tokens))
{
  fprintf(stderr, "%s:%d:%s (unparsed data: %s)\n",
	  str_asciz(filename), *lineno, errmsg, str_asciz(buffer));
}


enum cmod_status
cmod_tokenize(FILE *file,                      /* in */
	      const struct cmod_str *filename, /* in */
	      struct cmod_str *buffer,         /* in/out */
	      int *eof_seen,                   /* in/out */
	      int *lineno,                     /* in/out */
	      int *used_tokens,                /* in/out */
	      struct cmod_str **tokens,        /* in/out */
	      int *allocated_tokens)           /* in/out */
{
  int c = -1;
  int keep_c = 0;
  int digits;
  enum parse_state ps = st_hunt_token;
  enum quote_state qs = quote_none;

  *used_tokens = -1;

  while (1)
    {
      if (keep_c)
	keep_c = 0;
      else
	switch (next_char(file, filename, buffer, eof_seen, lineno, &c))
	  {
	  case cmod_ok:
	    break;

	  case cmod_eof:
	    c = EOF;
	    break;

	  case cmod_abort_module:
	    return cmod_abort_module;
	  }
      
      if (qs == quote_backslash && c == '\n')
	qs = quote_none;      /* Remove "backslash newline" sequences. */
      else if (ps == st_in_comment)
	{
	  if (c == '\n' || c == EOF)
	    {
	      keep_c = 1;
	      ps = st_hunt_token;
	    }
	}
      else if (ps == st_hunt_token)
	switch (c)
	  {
	  case EOF:
	    return (++*used_tokens == 0) ? cmod_eof : cmod_ok;
	  case '\n':
	    if (++*used_tokens > 0)
	      return cmod_ok;
	    *used_tokens = -1;
	    break;
	  case ' ':
	  case '\t':
	    break;
	  case '"':
	    append_token(used_tokens, tokens, allocated_tokens);
	    ps = st_in_str;
	    break;
	  case '#':
	    ps = st_in_comment;
	    break;
	  default:
	    append_token(used_tokens, tokens, allocated_tokens);
	    ps = st_in_token;
	    keep_c = 1;
	    break;
	  }
      else if (qs != quote_none && (ps == st_in_token || ps == st_in_str))
	{
	  if (qs == quote_backslash)
	    if (c == '"' || c == '#' || c == ' ' || c == '\\')
	      append_char(used_tokens, tokens, c);
	    else if (c == 'n')
	      append_char(used_tokens, tokens, '\n');
	    else if (c >= '0' && c <= '7')
	      {
		digits = 0100 * (c - '0');
		qs = quote_d1;
	      }
	    else
	      {
		tok_error("bad tokenizer escape sequence",
			  filename, buffer, lineno, used_tokens, tokens);
		return cmod_abort_module;
	      }
	  else if (qs == quote_d1 || qs == quote_d2)
	    if (c >= '0' && c <= '7')
	      if (qs == quote_d1)
		{
		  digits = 0010 * (c - '0');
		  qs = quote_d2;
		}
	      else
		{
		  digits = 0001 * (c - '0');
		  qs = quote_none;
		  append_char(used_tokens, tokens, digits);
		}
	    else
	      {
		tok_error("bad octal digit sequence",
			  filename, buffer, lineno, used_tokens, tokens);
		return cmod_abort_module;
	      }
	  else
	    assert(0);
	}
      else if (ps == st_in_token)
	if (c == ' ' || c == '\t')
	  ps = st_hunt_token;
	else if (c == EOF || c == '\n')
	  {
	    ps = st_hunt_token;
	    keep_c = 1;
	  }
	else if (c == '\\')
	  qs = quote_backslash;
	else if (c == '"')
	  {
	    tok_error("unquoted quote in token",
		      filename, buffer, lineno, used_tokens, tokens);
	    return cmod_abort_module;
	  }
	else if (c == '#')
	  ps = st_in_comment;
	else
	  append_char(used_tokens, tokens, c);
      else if (ps == st_in_str)
	if (c == '\\')
	  qs = quote_backslash;
	else if (c == EOF || c == '\n')
	  {
	    tok_error("unterminated string constant",
		      filename, buffer, lineno, used_tokens, tokens);
	    return cmod_abort_module;
	  }
	else if (c == '"')
	  ps = st_hunt_token;
	else
	  append_char(used_tokens, tokens, c);
      else
	assert(0);
    }
}
