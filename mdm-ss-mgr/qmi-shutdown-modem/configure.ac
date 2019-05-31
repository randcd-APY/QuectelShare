#                                               -*- Autoconf -*-

# configure.ac -- Autoconf script for qmi shutdowm modem
#

# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the qmi-framework package version 1.0.0
AC_INIT([qmi-shutdown-modem],1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
# Verifies the --srcdir is correct by checking for the path
AC_CONFIG_SRCDIR([qmi-shutdown-modem.c])
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

# Checks for libraries.
PKG_CHECK_MODULES([QMIF], [qmi-framework])
AC_SUBST([QMIF_CFLAGS])
AC_SUBST([QMIF_LIBS])

# Checks for libraries.
PKG_CHECK_MODULES([QMI], [qmi])
AC_SUBST([QMI_CFLAGS])
AC_SUBST([QMI_LIBS])

AC_ARG_ENABLE([target-apq],
    AC_HELP_STRING([--enable-target-apq],
        [Enable conditional compile for target apq]),
    [target_apq="${enableval}"],
    target_apq=no)

AM_CONDITIONAL([TARGET_APQ], [test "x$target_apq" = xyes])

AC_DEFINE(ENABLE_USEGLIB, 1, [Define if HLOS systems uses glib])
PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.16, dummy=yes, AC_MSG_ERROR(GLib >= 2.16 is required))
GLIB_LIBS="$GLIB_LIBS $GTHREAD_LIBS"
AC_SUBST(GLIB_LIBS)

AC_CONFIG_FILES([ \
        Makefile \
        ])
AC_OUTPUT
