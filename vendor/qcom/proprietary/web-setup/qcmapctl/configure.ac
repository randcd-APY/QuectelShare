#                                               -*- Autoconf -*-

# configure.ac -- Autoconf script for qcmapctl.
#

# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the qcmapctl package version 1.0.0
AC_INIT([qcmapctl],1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_MAKE_SET

# Checks for libraries.
PKG_CHECK_MODULES([DATA], [data])
AC_SUBST([DATA_CFLAGS])
AC_SUBST([DATA_LIBS])

PKG_CHECK_MODULES([DSUTILS], [dsutils])
AC_SUBST([DSUTILS_CFLAGS])
AC_SUBST([DSUTILS_LIBS])

PKG_CHECK_MODULES([QMI], [qmi])
AC_SUBST([QMI_CFLAGS])
AC_SUBST([QMI_LIBS])

PKG_CHECK_MODULES([QMI_FRAMEWORK], [qmi-framework])
AC_SUBST([QMI_FRAMEWORK_CFLAGS])
AC_SUBST([QMI_FRAMEWORK_LIBS])

AC_ARG_ENABLE([debug],
        [  --enable-debug    Turn on debugging],
        [case "${enableval}" in
            yes) debug=true ;;
            no)  debug=false ;;
            *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
        esac],[debug=false])

AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AC_CONFIG_FILES([ \
        Makefile \
        ])
AC_OUTPUT

