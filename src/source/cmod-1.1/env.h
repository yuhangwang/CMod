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

#ifndef ENV_H
#define ENV_H

/* An environment variable is stored in a "struct env" entry.  All
   codes that need access to the implementation of a "struct env"
   resides in env.c, where the internals of this struct is declared. */

struct env;


/* A collection of environment variables are collectively stored
   in a "struct envarray". */

struct envarray;


/*
 * "struct env" methods.
 */


/* Return true if FRAG is present as a SEP-separated fragment of E. */

#ifdef PROTOTYPES
extern int env_frag_present(struct env *e, char sep,
			    const struct cmod_str *frag);
#else
extern int env_frag_present();
#endif


/* Append a fragment to the variable.  No duplicate check is done.  It
   is the responsibility of the caller to ensure that FRAG doesn't
   contain the fragment delimiter SEP.  Ownership of the memory
   pointed to by FRAG is passed to E. */

#ifdef PROTOTYPES
extern void env_append_one_frag(struct env *e, char sep,
				const struct cmod_str *frag);
#else
extern void env_append_one_frag();
#endif


/* Split the SEP-separated fragments in FRAGS and append each frag to
   E.  If UNIQUE is true each fragment will only be added if it isn't
   already present. */

#ifdef PROTOTYPES
extern void env_append_frags(struct env *e, char sep,
			     const struct cmod_str *frags,
			     int unique);
#else
extern void env_append_frags();
#endif


/* Prepend a fragment to the variable.  No duplicate check is done.
   It is the responsibility of the caller to ensure that FRAG doesn't
   contain the fragment delimiter SEP. */

#ifdef PROTOTYPES
extern void env_prepend_one_frag(struct env *e, char sep,
				 struct cmod_str *frag);
#else
extern void env_prepend_one_frag();
#endif


/* Split the SEP-separated fragments in FRAGS and prepend each frag to
   E.  If UNIQUE is true any fragment already present in E will be
   removed before the fragment is prepended.  The first framgent on
   FRAGS will be the first fragment on E after the operation. */

#ifdef PROTOTYPES
extern void env_prepend_frags(struct env *e, char sep,
			      const struct cmod_str *frags,
			      int unique);
#else
extern void env_prepend_frags();
#endif


/* Remove all SEP-separated frags from E which are equal to FRAG. */

#ifdef PROTOTYPES
extern void env_remove_one_frag(struct env *e, char sep,
				const struct cmod_str *frag);
#else
extern void env_remove_one_frag();
#endif


/* Split the SEP-separated fragments in FRAGS and remove all fragments
   that are equal to any of them from E. */

#ifdef PROTOTYPES
extern void env_remove_frags(struct env *e, char sep,
			     const struct cmod_str *frags);
#else
extern void env_remove_frags();
#endif


/* Set the variable to a non-fragmented string.  Any previous value
   is lost, and the variable becomes non-fragmented. */

#ifdef PROTOTYPES
extern void env_setenv(struct env *e, const struct cmod_str *val);
#else
extern void env_setenv();
#endif

/* Make E explicitly unset. */

#ifdef PROTOTYPES
extern void env_unsetenv(struct env *e);
#else
extern void env_unsetenv();
#endif


/* Dump the value of E to F, readable for shell SH. */

#ifdef PROTOTYPES
extern void env_dump(struct env *e, FILE *f, const char *sh);
#else
extern void env_dump();
#endif


/* Iterate through all fragments.  The iterator can be used like this:

   int i;
   struct cmod_str fragment;
   str_ctor(&fragment);
   for (i = 0; env_iterate(e, &fragment, ':', &i); )
       printf("fragment: %s\n", str_asciz(&fragment));
   str_dtor(&fragment);

   The loop variable i is incremented each time env_iterate is called.
   env_iterate returns false when there are no more fragments to
   fetch. */

#ifdef PROTOTYPES
extern int env_iterate(struct env *e, struct cmod_str *frag,
		       char sep, int *iterator);
#else
extern int env_iterate();
#endif


/* Store the value of the variable E in string RES.
   RES should already be constructed.
   Note: The returned value is only valid until the next call to any
   method that may modify the variable.

   Return values:
     0 the variable has been set or modified by the program.  Note:
       this includes variables that was created by a call to
       envarray_locate_create with read_env=0 and auto_create=1.
     1 clean, that is, the value was read from the environment and has
       not been modified since.
     2 unset, that is, the variable has been explicitly unset.  RES
       will contain the empty string.
     3 unset and clean, that is, the variable was not set in the
       environment and it has not been modified since.  */

#ifdef PROTOTYPES
extern int env_value(struct env *e, struct cmod_str *res);
#else
extern int env_value();
#endif

/* Similary to env_value, but the value stored in RES contains
   the variable name followed by an equal sign and the actual value.
   This value is only stored if the function returns 0.

   If set, RES points to a NUL-terminated string (such as "PATH=/bin")
   which is valid only until the value of the variable changes.

   Return values: see env_value. */

#ifdef PROTOTYPES
extern int env_environ(struct env *e, const char **res);
#else
extern int env_environ();
#endif

/*
 * "struct envarray" methods.
 */


/* Get a pointer to the environment variable named NAME.  The contents of the
   variable will be read from the actual process environment if
   a variable named NAME is not alreay present.

   Warning: the returned value is only valid until the next time that
   envarray_locate_create is called. */

#ifdef PROTOTYPES
extern struct env *
envarray_locate_create(struct envarray *arr, const struct cmod_str *name);
#else
extern struct env *envarray_locate_create();
#endif


/* Get a pointer to the environment variable named NAME, or NULL if
   the variable doesn't exist.

   Warning: the returned value is only valid until the next time that
   envarray_locate_create is called.  The value remains valid as long
   as only envarray_locate is called, however.  So, if you want to
   access the values of two variables simultaneously you could do
   something like this:

      struct env *foo, *bar;
      foo = envarray_locate_create(arr, foo_str,1,1);
      bar = envarray_locate_create(arr, bar_str,1,1); // foo now points to junk
      foo = envarray_locate(arr, foo_str); // doesn't destroy bar

      // now, both foo and bar are valid
   */

#ifdef PROTOTYPES
extern struct env *envarray_locate(struct envarray* arr,
				   const struct cmod_str* name);
#else
extern struct env *envarray_locate();
#endif


/* Dump all variables in the environment ARR to file F using syntax
   appropriate for the shell SH.  Ignore variables that have been read
   from the environment but not modified. */

#ifdef PROTOTYPES
extern void envarray_dump(struct envarray *arr, FILE *f, const char *sh);
#else
extern void envarray_dump();
#endif


/* Call a callback function for each environment variable in ARR. */
#ifdef PROTOTYPES
extern void envarray_map(struct envarray *arr, void (*cb)(struct env*));
#else
extern void envarray_map();
#endif


/* Allocate and create an environment. */

#ifdef PROTOTYPES
extern struct envarray *create_envarray(void);
#else
extern struct envarray *create_envarray();
#endif


/* Destroy and deallocate an environment. */

#ifdef PROTOTYPES
extern void destroy_envarray(struct envarray *arr);
#else
extern void destroy_envarray();
#endif

#endif
