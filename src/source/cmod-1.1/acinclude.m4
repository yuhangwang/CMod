dnl  cmod - set up the user environment with module files.
dnl  Copyright (C) 1997, 1998 by the authors of cmod (see AUTHORS).
dnl
dnl  This program is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU General Public License as published by
dnl  the Free Software Foundation; either version 2 of the License, or
dnl  (at your option) any later version.
dnl
dnl  This program is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl  GNU General Public License for more details.
dnl
dnl  You should have received a copy of the GNU General Public License
dnl  along with this program; if not, write to the Free Software
dnl  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */
dnl
dnl Check if an option is acceptable to the C compiler in use.
dnl This definition is frozen; other packages are encouraged to
dnl copy this macro verbatim.  If there ever is a reason to change
dnl the semantics, even of the cache variables, the name of this macro
dnl must and will change.  (This macro is also used in isc).
dnl
AC_DEFUN(CMOD_COMPILER_CC_ACCEPTS,
[cmod_safe=`echo "cmod_cv_compiler_${CC}_accepts_$1" | tr '+./=\055' 'X____'`
AC_MSG_CHECKING([whether ${CC} accepts $1])
AC_CACHE_VAL($cmod_safe,
[[cmod_oldflags=$CFLAGS
CFLAGS="$CFLAGS $1"]
AC_TRY_LINK(,,
	[eval "$cmod_safe=yes"],
	[eval "$cmod_safe=no"])
[CFLAGS=$cmod_oldflags]])dnl
AC_MSG_RESULT([`eval echo '$'$cmod_safe`])
if test `eval echo '$'$cmod_safe` = yes; then
  CFLAGS="$CFLAGS $1"
fi])dnl
dnl
dnl
dnl sys/param.h and sys/types.h cannot both be included on ABCenix 5.18.
dnl
AC_DEFUN(CMOD_HEADER_PARAM,
[AC_CACHE_CHECK([whether sys/param.h and sys/types.h may both be included],
  cmod_cv_header_param,
[AC_TRY_COMPILE([#include <sys/param.h>
#include <sys/types.h>],
[], cmod_cv_header_param=yes, cmod_cv_header_param=no)])
[if test $cmod_cv_header_param = yes; then]
  AC_DEFINE(SYS_TYPES_WITH_SYS_PARAM)
[fi]])
