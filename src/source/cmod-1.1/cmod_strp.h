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


/* This file should only be included from cmod_string.h, which is the
   documented interface to this file.  This file exists for
   portability reasons only.  */


/*
 * Constructor/destructor
 */

/* Construct an empty string. */
extern void str_ctor(struct cmod_str *);

/* Deconstruct a string. */
extern void str_dtor(struct cmod_str *);

/* Create a new pointer to the string.  This is a copy constructor. */
extern void str_copy(struct cmod_str *copy, const struct cmod_str *orig);

/* Copy an array of strings. */
extern struct cmod_str *copy_str_array(int siz, struct cmod_str *arr);

/* Free an array of strings. */
extern void free_str_array(int siz, struct cmod_str *arr);

/*
 * Modifying the string
 */

/* Make the string contain the empty string. */
extern void str_clear(struct cmod_str *);

/* Append a NUL-terminated string. */
extern void str_append_asciz(struct cmod_str *, const char *);

/* Append a string. */
extern void str_append_str(struct cmod_str *, const struct cmod_str *);

/* Append a buffer. */
extern void str_append_buf(struct cmod_str *, const char *, long);

/* Append a character. */
extern void str_append_char(struct cmod_str *, int);

/* Set to a string.
   Note: this is not a constructor. */
extern void str_set_str(struct cmod_str *, const struct cmod_str *);

/* Truncate the string. */
extern void str_truncate(struct cmod_str *s, long size);

/* Remove the first N characters from the string. */
extern void str_remove_first(struct cmod_str *s, long size);

/* Remove the last N characters from the string. */
extern void str_remove_last(struct cmod_str *s, long size);

/*
 * Extracting information from the string
 */

/* Return a NUL-terminated string.  The return value is only valid
   until the next call to any funtion that operates on this string. */
extern const char *str_asciz(const struct cmod_str *);

/* Return the lenght of the string. */
extern long str_strlen(const struct cmod_str *);

/* Return true if the string is empty (has length zero). */
extern int str_empty(const struct cmod_str *);

/* Return the index of the first occurance of the character C, or -1
   if it isn't present in the string. */
extern long str_strchr(const struct cmod_str *, char c);

/* See strcmp(3). */
extern int str_strcmp(const struct cmod_str *, const struct cmod_str *);

/* Return the last character in the string as an unsigned value.
   Return -1 if the string is empty. */
extern int str_tail(const struct cmod_str *);
