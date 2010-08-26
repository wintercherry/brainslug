#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_LANG(C++)
AC_PREREQ([2.65])
AC_INIT([Brainslug-Backend], [0.1], [michel.boto@gmail.com])
#AC_CONFIG_SRCDIR([include/Conf.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
BOOST_REQUIRE
BOOST_SYSTEM
BOOST_THREADS
BOOST_PROGRAM_OPTIONS
BOOST_ASIO
BOOST_FILESYSTEM

#AC_CHECK_LIB([crypto], [BF_set_key])
AC_CHECK_LIB([sqlite3], [sqlite3_open])
#AC_CHECK_LIB([ssl], [SSL_CTX_free])
#AC_CHECK_LIB([pion-common], [write])
AC_CHECK_LIB([pion-net], [write])
# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
#AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

AC_CONFIG_FILES([Makefile
		 src/Makefile
		 lib/Makefile])
AC_OUTPUT
