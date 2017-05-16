#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(prokgc, 0.0.1, samuel.barreto8@gmail.com)
AC_CONFIG_SRCDIR([src/prokgc.c])
AC_CONFIG_FILES([Makefile examples/Makefile src/Makefile])
AM_INIT_AUTOMAKE

: ${CFLAGS="-g -Wall -O3 -std=gnu99"}
# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([DEPS], [glib-2.0 >= 2.50.0])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC



AC_OUTPUT
