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
#ifdef HAVE_GETWD
#  include <sys/param.h>
#  ifdef SYS_TYPES_WITH_SYS_PARAM
#    include <sys/types.h>
#  endif
#else
#  include <sys/types.h>
#endif
#include <sys/stat.h>
#include <unistd.h>

#include "cmod_string.h"
#include "env.h"
#include "cmod_dirent.h"
#include "cmod_main.h"
#include "cmod_trace.h"
#include "cmod_parse.h"
#include "cmod_alloc.h"
#include "unused.h"
#include "archaic.h"


/* Dynix 3.1.1 fails to define S_ISDIR and S_ISLNK. */
#ifndef S_ISDIR
#  define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISLNK
#  define S_ISLNK(mode) (((mode) & S_IFMT) == S_IFLNK)
#endif

/* Struct for "module avail" */
struct avail_info {
  struct cmod_str name;		/* Includes the directory, if any. */
  int is_default;		/* True if this module is the default
				   version of a module that is a
				   directory. */
};


/* Forward declarations. */
#ifdef PROTOTYPES
static int parse_module(FILE *fp,
			const struct cmod_str *filename,
			int for_removal,
			int *lineno);
#else
static int parse_module();
#endif

/* Random globals */

const char *argv0 = NULL;
static int cmod_errors = 0;
struct envarray *global_env = NULL;
static int warning_found = 0;

static struct cmod_str const_modulepath;
static struct cmod_str const_loadedmodules;

#define DEFAULT_MODULE "default"


static void
global_ctors(void)
{
  str_ctor(&const_modulepath);
  str_append_asciz(&const_modulepath, "MODULEPATH");
  str_ctor(&const_loadedmodules);
  str_append_asciz(&const_loadedmodules, "LOADEDMODULES");
}


static void
global_dtors(void)
{
  str_dtor(&const_modulepath);
  str_dtor(&const_loadedmodules);
}


static void
warning(const struct cmod_str *filename,
	const int *lineno,
	const char *msg)
{
  warning_found = 1;
  if (filename != NULL && lineno != NULL)
    fprintf(stderr, "%s:%d: warning: %s\n", str_asciz(filename), *lineno, msg);
  else
    fprintf(stderr, "module: warning: %s\n", msg);
}


static void
error(const struct cmod_str *filename,
      const int *lineno,
      const char *msg)
{
  if (filename != NULL && lineno != NULL)
    fprintf(stderr, "%s:%d: %s", str_asciz(filename), *lineno, msg);
  else
    fprintf(stderr, "module: %s\n", msg);
}


static int
append_path(const struct cmod_str *UNUSED(module),
	    const int *UNUSED(line),
	    int argc,
	    const struct cmod_str *argv)
{
  struct env *e;

  assert(argc == 2);

  e = envarray_locate_create(global_env, &argv[0]);
  env_append_frags(e, ':',&argv[1] , 1);
  
  return 0;
}


static int
prepend_path(const struct cmod_str *UNUSED(module),
	     const int *UNUSED(line),
	     int argc,
	     const struct cmod_str *argv)
{
  struct env *e;

  assert(argc == 2);

  e = envarray_locate_create(global_env, &argv[0]);
  env_prepend_frags(e, ':', &argv[1], 1);
  return 0;
}


static int
remove_path(const struct cmod_str *UNUSED(module),
	    const int *UNUSED(line),
	    int argc,
	    const struct cmod_str *argv)
{
  struct env *e;
  
  assert(argc == 2);

  e = envarray_locate_create(global_env, &argv[0]);
  env_remove_frags(e, ':', &argv[1]);
  return 0;
}


static int
set_env(const struct cmod_str *UNUSED(module),
	const int *UNUSED(line),
	int argc,
	const struct cmod_str *argv)
{
  struct env *e;

  assert(argc == 2);

  e = envarray_locate_create(global_env, &argv[0]);
  env_setenv(e, &argv[1]);
  return 0;
}


static int
unset_env(const struct cmod_str *UNUSED(module),
	  const int *UNUSED(line),
	  int argc,
	  const struct cmod_str *argv)
{
  struct env *e;

  assert(argc == 2 || argc == 1); /* Ignore the value, if any. */

  e = envarray_locate_create(global_env, &argv[0]);
  env_unsetenv(e);
  return 0;
}


static void
canonicalize_path(struct cmod_str *path)
{
#ifndef HAVE_GETWD
  char *cwd;
  char *res;
  int size = 256;
#else
  char cwd[MAXPATHLEN];
#endif
  
  if (str_asciz(path)[0] != '/')
    {
      struct cmod_str fullpath;
#ifndef HAVE_GETWD
      cwd = xmalloc(size);
      while ((res = getcwd(cwd, size)) == NULL && errno == ERANGE)
	{
	  size *= 2;
	  cwd = xrealloc(cwd, size);
	}
      if (res == NULL)
	{
	  fprintf(stderr, "%s: while determining PWD", argv0);
	  perror("");
	  exit(CMOD_EXIT_ERROR);
	}
#else
      if (getwd(cwd) == NULL)
	{
	  fprintf(stderr, "%s: while determining PWD", argv0);
	  perror("");
	  exit(CMOD_EXIT_ERROR);
	}
#endif
      str_ctor(&fullpath);
      str_append_asciz(&fullpath, cwd);
      str_append_char(&fullpath, '/');
      str_append_str(&fullpath, path);
      str_set_str(path, &fullpath);
      str_dtor(&fullpath);
#ifndef HAVE_GETWD
      xfree(cwd);
#endif
    }
  /* Future improvement(?):   /a/b/../c  ->  /a/c */
}

static void
do_use_path(const struct cmod_str *path)
{
  struct env *e = envarray_locate_create(global_env, &const_modulepath);
  struct cmod_str fullpath;
  str_copy(&fullpath, path);
  canonicalize_path(&fullpath);
  env_prepend_frags(e, ':', &fullpath, 1);
  str_dtor(&fullpath);
}

static int
use_path(const struct cmod_str *UNUSED(containing_filename),
	 const int *UNUSED(containing_lineno),
	 int argc,
	 const struct cmod_str *argv)
{
  assert(argc == 1);
  do_use_path(&argv[0]);
  return 0;
}


static void
do_unuse_path(const struct cmod_str *path)
{
  struct env *e = envarray_locate_create(global_env, &const_modulepath);
  struct cmod_str fullpath;
  str_copy(&fullpath, path);
  canonicalize_path(&fullpath);
  env_remove_frags(e, ':', &fullpath);
  str_dtor(&fullpath);
}

static int
unuse_path(const struct cmod_str *UNUSED(containing_filename),
	   const int *UNUSED(containing_lineno),
	   int argc,
	   const struct cmod_str *argv)
{
  assert(argc == 1);
  do_unuse_path(&argv[0]);
  return 0;
}


/* Open MODULE for reading and return a FILE*.  Search for MODULE in
   the environment variable MODULEPATH (which is stored in the global
   environment if it wasn't already present there).  The file name is
   returned in FOUND_FILENAME.  Ownership of the filename passes to
   the caller.

   If MODULE turns out to be a directory, this will open
   MODULE/default if such a file exists.  Otherwise this function will
   fail.

   Prints an error message and returns NULL if the module could not be
   opened.  However, if dont_report_missing is true and the module
   cannot be found no error message is printed (but an error message
   is still printed if the module was found but cannot be opened).  */

static FILE *
open_module(const struct cmod_str *module,
	    struct cmod_str *found_filename,
	    int dont_report_missing,
	    const struct cmod_str *containing_filename,
	    const int *containing_lineno)
{
  int component = 0;
  struct cmod_str dir;		/* The current directory we search. */
  FILE *fp;
  struct env *module_path;
  struct cmod_str fn;		/* Complete file name of module file. */
  struct stat statbuf;

  str_ctor(&dir);
  str_ctor(&fn);

  module_path = envarray_locate_create(global_env, &const_modulepath);

  while (env_iterate(module_path, &dir, ':', &component))
    {
      str_clear(&fn);
      str_append_str(&fn, &dir);
      str_append_char(&fn, '/');
      str_append_str(&fn, module);
      
      if (stat(str_asciz(&fn), &statbuf) == 0)
	{
	  long old_size = str_strlen(&fn);

	  if (S_ISDIR(statbuf.st_mode))
	    {
	      str_append_char(&fn, '/');
	      str_append_asciz(&fn, DEFAULT_MODULE);
	      if (stat(str_asciz(&fn), &statbuf) == 0)
		{
#ifndef STAT_MACROS_BROKEN
		  if (S_ISDIR(statbuf.st_mode))
		    {
		      fprintf(stderr,
			      "%s: opening module %s: %s is a directory\n",
			      argv0, str_asciz(module), str_asciz(&fn));
		      str_dtor(&dir);
		      str_dtor(&fn);
		      return NULL;
		    }
#endif
		  fp = fopen(str_asciz(&fn), "r");
		  if (fp != NULL)
		    {
		      str_set_str(found_filename, &fn);
		      str_dtor(&dir);
		      str_dtor(&fn);
		      return fp;
		    }
		  else
		    {
		      fprintf(stderr, "%s: opening ", argv0);
		      perror(str_asciz(&fn));
		      str_dtor(&dir);
		      str_dtor(&fn);
		      return NULL;
		    }
		}
	      else if (errno != ENOENT)
		{
		  fprintf(stderr, "%s: while statting ", argv0);
		  perror(str_asciz(&fn));
		  str_dtor(&dir);
		  str_dtor(&fn);
		  return NULL;
		}
	      else
		{
		  str_truncate(&fn, old_size);
#ifdef STAT_MACROS_BROKEN
		  /* dir/module might not be a directory after all.
		     Fall through to the code below that tries to open
		     fn as a plain file.  This means that the error
		     message "dir lacks dir/default" will never be
		     displayed on hosts that with STAT_MACROS_BROKEN,
		     which is unfortunate, but at least a proper setup
		     will work on them this way. */
#else
		  fprintf(stderr, "%s: %s lacks %s/default\n", argv0,
			  str_asciz(&fn), str_asciz(&fn));
		  str_dtor(&dir);
		  str_dtor(&fn);
		  return NULL;
#endif
		}
	    }

	  /* If we get here, assume that fn contains a file name.
	     If STAT_MACROS_BROKEN is true, it may actually be a
	     directory. */

	  errno = 0;
	  fp = fopen(str_asciz(&fn), "r");
	  if (fp == NULL)
	    {
#ifdef EISDIR
	      if (errno == EISDIR)
		{
		  str_truncate(&fn, old_size);
		  fprintf(stderr, "%s: %s lacks %s/default\n", argv0,
			  str_asciz(&fn), str_asciz(&fn));
		  str_dtor(&dir);
		  str_dtor(&fn);
		  return NULL;
		}
#endif
	      /* Strange.  We could stat this file, but we cannot open
		 it.  Maybe we don't have permission to read it. */
	      fprintf(stderr, "%s: opening ", argv0);
	      perror(str_asciz(&fn));
	      str_dtor(&dir);
	      str_dtor(&fn);
	      return NULL;
	    }
	  
	  str_set_str(found_filename, &fn);
	  str_dtor(&dir);
	  str_dtor(&fn);
	  return fp;
	}
      else if (errno != ENOENT)
	{
	  fprintf(stderr, "%s: while statting ", argv0);
	  perror(str_asciz(&fn));
	  str_dtor(&dir);
	  str_dtor(&fn);
	  return NULL;
	}
    }

  str_dtor(&dir);
  str_dtor(&fn);

  if (!dont_report_missing)
    {
      error(containing_filename, containing_lineno, "failed to find ");
      fprintf(stderr, "%s in $MODULEPATH.\n", str_asciz(module));
    }

  return NULL;
}

  
/* Return values:
     0 ok
     1 module not found */
static int
do_rm_module(const struct cmod_str *containing_filename,
	     const int *containing_lineno,
	     const struct cmod_str *module_name)
{
  struct cmod_str fn;
  int lineno = 0;
  FILE *mfp;

  str_ctor(&fn);

  if (env_frag_present(envarray_locate_create(global_env,
					      &const_loadedmodules),
		       ':', module_name))
    {
      mfp = open_module(module_name, &fn, 0,
			containing_filename, containing_lineno);
      if (mfp == NULL)
	return 1;

      parse_module(mfp, &fn, 1, &lineno);
      env_remove_one_frag(envarray_locate_create(global_env,
						 &const_loadedmodules),
			  ':', module_name);
      fclose(mfp);
    }
  else
    warning(containing_filename, containing_lineno,
	    "ignored attempt to unload unloaded module.");

  str_dtor(&fn);
  return 0;
}

static int
rm_module(const struct cmod_str *containing_filename,
  const int *containing_lineno,
	  int argc,
	  const struct cmod_str *argv)
{
  int opti;

  for (opti = 0; opti < argc; ++opti)
    if (do_rm_module(containing_filename, containing_lineno, &argv[opti]))
      return 1;
  return 0;
}


/* Retval:
     0 module found and processed (either loaded OK or aborted)
     1 module not found.  */
static int
do_add_module(const struct cmod_str *containing_filename,
	      const int *containing_lineno,
	      const struct cmod_str *module_name,
	      int dont_report_missing)
{
  int lineno = 0;
  FILE *mfp;
  struct cmod_str fn;
  struct env *loaded_modules;

  str_ctor(&fn);

  if (env_frag_present(envarray_locate_create(global_env,
					      &const_loadedmodules),
		       ':', module_name))
    do_rm_module(containing_filename, containing_lineno, module_name);

  mfp = open_module(module_name, &fn, dont_report_missing,
		    containing_filename, containing_lineno);
  if (mfp == NULL)
    {
      str_dtor(&fn);
      return 1;
    }

  if (parse_module(mfp, &fn, 0, &lineno) == 0)
    {
      /* We have successfully loaded the module, so add it to the list
	 of loaded modules.  */
      loaded_modules = envarray_locate_create(global_env,
					      &const_loadedmodules);
      env_append_one_frag(loaded_modules, ':', module_name);
    }
  else
    cmod_errors = 1;

  fclose(mfp);
  str_dtor(&fn);

  return 0;
}


static int
add_module(const struct cmod_str *containing_filename,
	   const int *containing_lineno,
	   int argc,
	   const struct cmod_str *argv)
{
  int opti;

  for (opti = 0; opti < argc; ++opti)
    if (do_add_module(containing_filename, containing_lineno, &argv[opti], 0))
      return 1;

  return 0;
}


static int
add_module_if_present(const struct cmod_str *containing_filename,
		      const int *containing_lineno,
		      int argc,
		      const struct cmod_str *argv)
{
  int opti;

  for (opti = 0; opti < argc; ++opti)
    do_add_module(containing_filename, containing_lineno, &argv[opti], 1);

  return 0;
}



static int
list_modules(const struct cmod_str *UNUSED(from_module),
	     const int *UNUSED(from_line),
	     int argc,
	     const struct cmod_str *UNUSED(argv))
{
  struct env *loaded_modules;
  int i;
  struct cmod_str module;

  str_ctor(&module);

  assert(argc == 0);

  loaded_modules = envarray_locate_create(global_env, &const_loadedmodules);

  fprintf(stderr,"Currently loaded modules:\n");
  for (i = 0; env_iterate(loaded_modules, &module, ':', &i); )
    fprintf(stderr,"%3d) %s\n", i, str_asciz(&module));

  str_dtor(&module);

  return 0;
}

static int
no_op(const struct cmod_str *UNUSED(module),
      const int *UNUSED(line),
      int UNUSED(argc),
      const struct cmod_str *UNUSED(argv))
{
  return 0;
}


static int
is_module_name(const struct cmod_str *name)
{
  /* Avoid recursion. */
  if (!strcmp(str_asciz(name), "."))
    return 0;
  if (!strcmp(str_asciz(name), ".."))
    return 0;

  /* Anything that ends in a tilde is probably an Emacs autosave file.
     Avoid clutter. */
  if (str_strlen(name) > 0 && str_asciz(name)[str_strlen(name)-1] == '~')
    return 0;

  return 1;
}

static void
avail_modules_recursively(const struct cmod_str *moduledir,
			  const struct cmod_str *subdir,
			  const char *default_module,

			  struct avail_info **res,
			  int *nr_modules,
			  int *res_size,

			  int maxdepth)
{
  DIR *dp;
  struct dirent *entry;
  struct cmod_str directory;
  struct cmod_str filename;
  struct cmod_str tentative_module;
  struct stat statbuf;
  struct avail_info *info_entry;

#ifdef HAVE_READLINK
  struct cmod_str default_buf;
  char *symlink_val = NULL;
  /* symlink_size is the current size of the symlink_val buffer.  (We
     only allocate the buffer when we encounter a symlink).  We could
     make this arbitrarily small, since we reallocate the buffer if we
     encounter a large symlink.  Unfortunately, there are systems
     where readlink returns -1 and sets errno to EINVAL (PowerMAX OS
     4.1 when the symlink is present on a local file system is one
     example) or EIO (e.g. Linux 2.0.33 when the symlink is present on
     a NFS mounted file system).  We don't handle those cases, except
     that we use a large enough buffer so that we in practice should
     never have to reallocate it. */
  int symlink_size = 1012;
  int n;

#endif

  str_ctor(&directory);
  str_append_str(&directory, moduledir);
  if (subdir != NULL)
    {
      str_append_char(&directory, '/');
      str_append_str(&directory, subdir);
    }

  if (maxdepth <= 0)
    {
      fprintf(stderr, "%s: %s: too deep directory hierarchy\n",
	      argv0, str_asciz(&directory));
      str_dtor(&directory);
      return;
    }

  if ((dp = opendir(str_asciz(&directory))) == NULL)
    {
      fprintf(stderr, "%s: opening directory ", argv0);
      perror(str_asciz(&directory));
      str_dtor(&directory);
      return;
    }

  str_ctor(&filename);
  str_append_str(&filename, &directory);
  str_append_char(&filename, '/');

  str_ctor(&tentative_module);

#ifdef HAVE_READLINK
  str_ctor(&default_buf);
#endif

  while (errno = 0, (entry = readdir(dp)) != NULL)
    {
      str_clear(&tentative_module);
      str_append_buf(&tentative_module, entry->d_name, NAMLEN(entry));
      if (is_module_name(&tentative_module))
	{
	  str_truncate(&filename, str_strlen(&directory) + 1);
	  str_append_str(&filename, &tentative_module);

	  if (stat(str_asciz(&filename), &statbuf) != 0)
	    {
	      fprintf(stderr, "%s: while statting ", argv0);
	      perror(str_asciz(&filename));
	      exit(CMOD_EXIT_ERROR);
	    }

	  /* Ignore STAT_MACROS_BROKEN here -- if they are, "module
	     avail" simply won't work.  It is enough that the more
	     vital parts of "cmod" tries to work. */
	  if (S_ISDIR(statbuf.st_mode))
	    {
#ifdef HAVE_READLINK

	      str_clear(&default_buf);
	      str_append_str(&default_buf, &filename);
	      str_append_char(&default_buf, '/');
	      str_append_asciz(&default_buf, DEFAULT_MODULE);
	      if (lstat(str_asciz(&default_buf), &statbuf) == 0
		  && S_ISLNK(statbuf.st_mode))
		{
		  if (symlink_val == NULL)
		    symlink_val = xmalloc(symlink_size);
		  while (1)
		    {
		      /* Red Hat Linux 5.0 (and other versions of
			 Linux) will write a NUL character past the
			 buffer, so supply symlink_size-1 instead of
			 symlink_size here.  This will waste one
			 character on all other systems, but better
			 safe than sorry. */

		      n = readlink(str_asciz(&default_buf),
				   symlink_val, symlink_size-1);
		      if (n >= 0 && n < symlink_size-1)
			{
			  /* We got the entire symlink, with room to
			     spare.  NUL-terminate it, and exit from
			     this loop.  This is the normal exit from
			     the loop. */
			  symlink_val[n] = '\0';
			  break;
			}
		      else if (n < 0 && (errno != ERANGE))
			{
			  fprintf(stderr, "%s: readlink symlink ", argv0);
			  perror(str_asciz(&default_buf));
			  exit(1);
			}

		      /* There wasn't room for the symlink.
			 Reallocate and try again. */

		      symlink_size *= 2;
		      symlink_val = xrealloc(symlink_val, symlink_size);
		    }

		  /* OK, we've got the NUL-terminated symlink value
		     in symlink_val. */
		  /* If we should ever recurse more than one level
		     deep we should pass subdir+tentative_module
		     instead of only tentative_module. */
		  avail_modules_recursively(moduledir, &tentative_module,
					    symlink_val,
					    res, nr_modules, res_size,
					    maxdepth - 1);

		}
	      else
		{
		  /* The default file, if it exists, is not a symlink. */
		  avail_modules_recursively(moduledir, &tentative_module, NULL,
					    res, nr_modules, res_size,
					    maxdepth - 1);
		}
#else  /* symlinks not supported */
	      avail_modules_recursively(moduledir, &tentative_module, NULL,
					res, nr_modules, res_size,
					maxdepth - 1);
#endif
	    }
	  else
	    {
	      if (*nr_modules >= *res_size)
		{
		  ++*res_size;
		  *res = xrealloc(*res, *res_size * sizeof(struct avail_info));
		  str_ctor(&(*res)[*res_size - 1].name);
		}
	      info_entry = &(*res)[(*nr_modules)++];
	      if (subdir != NULL)
		{
		  str_set_str(&info_entry->name, subdir);
		  str_append_char(&info_entry->name, '/');
		  str_append_str(&info_entry->name, &tentative_module);
		}
	      else
		str_set_str(&info_entry->name, &tentative_module);

	      if (default_module != NULL
		  && !strcmp(str_asciz(&tentative_module), default_module))
		info_entry->is_default = 1;
	      else
		info_entry->is_default = 0;
	    }
	}
    }

  if (errno != 0)
    {
      fprintf(stderr, "%s: reading directory ", argv0);
      perror(str_asciz(&directory));
    }

#ifdef CLOSEDIR_VOID
  closedir(dp);
#else

again:
  if (closedir(dp) < 0)
    {
      if (errno == EINTR)
	goto again;
      fprintf(stderr, "%s: closing directory ", argv0);
      perror(str_asciz(&directory));
    }
#endif

#ifdef HAVE_READLINK
  xfree(symlink_val);
  str_dtor(&default_buf);
#endif
  str_dtor(&directory);
  str_dtor(&tentative_module);
  str_dtor(&filename);
}


static int
cmp_avail_infos(const void *a, const void *b)
{
  const struct avail_info *x = (const struct avail_info*)a;
  const struct avail_info *y = (const struct avail_info*)b;

  return strcmp(str_asciz(&x->name),
		str_asciz(&y->name));
}


static void
display_modules(const struct cmod_str *dir,
		const struct avail_info *modules,
		int n_modules,
		struct env *loaded_modules)
{
  int cols_used;
  int rows_used;
  int col;
  int row;
  long maxlen;
  long len;
  
  int ix;

  fprintf(stderr, "\nIn directory %s:\n\n", str_asciz(dir));

  for (maxlen = 0, ix = 0; ix < n_modules; ++ix)
    {
      len = (str_strlen(&modules[ix].name)
	     + 7 * !!modules[ix].is_default);
      if (len > maxlen)
	maxlen = len;
    }
  /* FIXME: should not hard-code the screen width.  True, 80
     characters is the only sane screen width, but the world is
     full of insane individuals.  For now, they will have to
     suffer. */
#define SCREEN_WIDTH 80
  /* The format of each module entry is:
       SPACE SPACE PLUS-OR-MINUS NAME DEFopt
     DEFopt is " (def) " if this is a default module. */
  cols_used = SCREEN_WIDTH / (maxlen + 3);
  if (cols_used < 1)
    cols_used = 1;
  rows_used = (n_modules-1) / cols_used + 1;
  for (row = 0; row < rows_used; ++row)
    {
      for (col = 0;
	   col < cols_used && row + col*rows_used < n_modules;
	   ++col)
	{
	  const struct avail_info *entry = &modules[row+col*rows_used];
	  putc(' ', stderr);
	  putc(' ', stderr);
	  if (env_frag_present(loaded_modules, ':', &entry->name))
	    putc('+', stderr);
	  else
	    putc('-', stderr);
	  if (entry->is_default)
	    fprintf(stderr, "%s (def) %*s", str_asciz(&entry->name),
		    (int)(maxlen - str_strlen(&entry->name) - 7), "");
	  else
	    fprintf(stderr, "%-*s", (int)maxlen, str_asciz(&entry->name));
	}
      putc('\n', stderr);
    }
}


static int
avail_modules(const struct cmod_str *UNUSED(from_module),
	      const int *UNUSED(from_line),
	      int argc,
	      const struct cmod_str *UNUSED(argv))
{
  struct env *loaded_modules;
  struct env *module_path;

  int component = 0;
  struct cmod_str dir;

  struct avail_info *available_modules = NULL;
  int modules_present;
  int modules_allocated = 0;

  assert(argc == 0);

  str_ctor(&dir);

  envarray_locate_create(global_env, &const_loadedmodules);
  module_path = envarray_locate_create(global_env, &const_modulepath);
  loaded_modules = envarray_locate(global_env, &const_loadedmodules);

  while (env_iterate(module_path, &dir, ':', &component))
    {
      modules_present = 0;
      avail_modules_recursively(&dir, NULL, NULL, &available_modules,
				&modules_present, &modules_allocated, 2);

      qsort(available_modules, modules_present, sizeof(struct avail_info),
	    &cmp_avail_infos);
  
      display_modules(&dir, available_modules, modules_present,
		      loaded_modules);
    }

  return 0;
}


static int
cmod_version(const struct cmod_str *UNUSED(from_module),
	     const int *UNUSED(from_line),
	     int UNUSED(argc),
	     const struct cmod_str *UNUSED(argv))
{
  fprintf(stderr,
	  "%s version %s.\n\
Copyright (C) 1997-1998 Per Cederqvist, Kent Engstr\366m and \
Henrik Rindl\366w.\n",
	  PACKAGE, VERSION);
  return 0;
}


static const struct cmd cmd_table[] = {
  {"append-path", NULL, 2, 2, append_path, remove_path},
  {"prepend-path", NULL, 2, 2, prepend_path, remove_path},
  {"remove-path", NULL, 2, 2, remove_path, remove_path},
  {"setenv", NULL, 2, 2, set_env, unset_env},
  {"unsetenv", NULL, 1, 1, unset_env, unset_env},
  {"module", "add", 1, -1, add_module, rm_module},
  {"module", "load", 1, -1, add_module, rm_module},
  {"module", "try-add", 1, -1, add_module_if_present, rm_module},
  {"module", "unload", 1, -1, rm_module, add_module},
  {"module", "rm", 1, -1, rm_module, add_module},
  {"module", "use", 1, 1, use_path, unuse_path},
  {"module", "unuse", 1, 1, unuse_path, use_path},
  {"module", "list", 0, 0, list_modules, no_op},
  {"module", "avail", 0, 0, avail_modules, no_op},
  {"module", "--version", 0, 0, cmod_version, no_op},
  {NULL, NULL, 0, 0, NULL, NULL}
};

static int
parse_module(FILE *fp,
	     const struct cmod_str *filename,
	     int for_removal,
	     int *lineno)
{
  return cmod_parse_module(fp, filename, for_removal, cmd_table, lineno);
}


static void
usage(void)
{
  fprintf(stderr,
	  "Usage: module sub-command [args...]\n\n");
  fprintf(stderr,
	  "  Available sub-commands:\n\n");
  fprintf(stderr,
	  "    add|load  modulefile [modulefile...]    Add module(s)\n");
  fprintf(stderr,
	  "    try-add   modulefile [modulefile...]    Add, don't complain if not found\n");
  fprintf(stderr,
	  "    rm|unload modulefile [modulefile...]    Remove module(s)\n");
  fprintf(stderr,
	  "    use       dir                           Add to MODULEPATH\n");
  fprintf(stderr,
	  "    unuse     dir                           Remove from MODULEPATH\
\n");
  fprintf(stderr,
	  "    list                                    List loaded modules\n");
  fprintf(stderr,
	  "    avail                                   List available modules\n");
  fprintf(stderr,
	  "    --version                               Show version information\n");
  fprintf(stderr,"\n");

  global_dtors();

  exit(CMOD_EXIT_ERROR);
}


int
main(int argc,
     const char **argv)
{
  argv0 = argv[0];

  global_ctors();
#if 0
  /* FIXME: Add a switch to enable selected traces. */
  set_traces(TRACE_ASSIGN);
#endif

  if (argc < 2)
    usage();

  if (!strcmp(argv[1], "sh") || !strcmp(argv[1], "csh"))
    {
    }
  else
    usage();

  global_env = create_envarray();

  switch (cmod_parse_argv(cmd_table, argc-2, argv+2))
    {
    case -1:
      /* bad usage */
      usage();
    case 0:
      /* no error */
      break;
    default:
      /* error (but command line looks ok) */
      cmod_errors = 1;
      break;
    }

  envarray_dump(global_env, stdout, argv[1]);
  destroy_envarray(global_env);

  global_dtors();

  if (cmod_errors)      
    exit(CMOD_EXIT_ERROR);
  if (warning_found)
    exit(CMOD_EXIT_WARNING);
  else
    exit(0);
}
