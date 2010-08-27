#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_LANG(C++)
AC_PREREQ([2.65])
AC_INIT([Theron], [0.0.0], [michel.boto@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
BOOST_REQUIRE
BOOST_THREADS

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 Source/Makefile])
AC_OUTPUT
