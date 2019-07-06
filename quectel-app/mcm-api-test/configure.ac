#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(mcm-api-test,1.0.0)
AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

AC_CONFIG_SRCDIR([src/Makefile.am])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC

# Checks for libraries.
# Checks for header files.
#PKG_CHECK_MODULES([QMI], [qmi])
#AC_SUBST([QMI_CFLAGS])

#PKG_CHECK_MODULES([QMIFRAMEWORK], [qmi-framework])
#AC_SUBST([QMIFRAMEWORK_CFLAGS])

#PKG_CHECK_MODULES([DATA], [data])
#AC_SUBST([DATA_CFLAGS])
#AC_SUBST([DATA_LIBS])

PKG_CHECK_MODULES([MCMCORE], [mcm-core])
AC_SUBST([MCMCORE_CFLAGS])
AC_SUBST([MCMCORE_LIBS])


# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([
	Makefile
    src/Makefile
])
AC_OUTPUT
