#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_LANG(C++)
AC_PREREQ([2.65])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([include/Conf.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

BOOST_REQUIRE 1.43
BOOST_SYSTEM
BOOST_THREADS
BOOST_PROGRAM_OPTIONS

# FIXME: Replace `main' with a function in `-lboost_program_options':
#AC_CHECK_LIB([boost_program_options], [boost::program_options::parse_command_line])
# FIXME: Replace `main' with a function in `-lboost_system':
#AC_CHECK_LIB([boost_system], [boost::system::system_error::errc::hash_value])
#AC_LANG_PROGRAM([#include <boost/system/error_code.hpp>],[boost::system::errc:hash_value(boost::system::errc::owner_dead)])

# FIXME: Replace `main' with a function in `-lcrypto':
AC_CHECK_LIB([crypto], [BF_set_key])
# FIXME: Replace `main' with a function in `-lsqlite3':
AC_CHECK_LIB([sqlite3], [sqlite3_open])
# FIXME: Replace `main' with a function in `-lssl':
AC_CHECK_LIB([ssl], [ssl_write])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([select])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
