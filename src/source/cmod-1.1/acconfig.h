/* Does the compiler understand __attribute__((unused))?
   This is used by gcc to indicate that a parameter to a function is
   unused, like this:
   .
         int foo(int x __attribute__((unused)))
         {
             // code that doesn't use the variable x
         }
   .
   Define this to one if the compiler understands it.  It will then
   emit fewer warnings about unused arguments. */
#undef HAVE_ATTRIBUTE_UNUSED

/* Is errno declared in <errno.h>?  Some ancient systems don't declare
   it. */
#undef HAVE_ERRNO_IN_ERRNO_H

/* Does the compiler understand ANSI-style prototypes? */
#undef PROTOTYPES

/* Does the compiler understand "static inline" as in "static inline
   int foo() { return bar(); }"?  If it does, define static_inline to
   "static inline", otherwise to "static" (or, possibly, "inline").  */
#define static_inline static

/* The name and version of this software distribution.  These
   originates from AM_INIT_AUTOMAKE in the configure.in file.  */
#define PACKAGE "configure-generated"
#define VERSION "configure-generated"

/* Can <sys/types.h> and <sys/param.h> both be included? */
#undef SYS_TYPES_WITH_SYS_PARAM

/* Full path (as a string) to the pwd program.  This is only used if
   neither getcwd nor getwd is found in libc. */
#undef PWD_PATH
