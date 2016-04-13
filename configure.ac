#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rfswd], [0.1], [canna.bissen@rocketmail.com])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_PROG_AR

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h memory.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/param.h sys/socket.h unistd.h errno.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([gethostbyname gethostname inet_ntoa memset socket strerror])
AC_REPLACE_FUNCS(strerror)

AC_OUTPUT([
Makefile
src/Makefile
src/socket/Makefile
src/thread/Makefile
src/file/Makefile
man/Makefile
test/Makefile
doc/Makefile
])



