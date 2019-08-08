AC_PREREQ(2.61)
AC_INIT([mcm-core], 1.0.0)
AM_INIT_AUTOMAKE([-Wall gnu foreign])
AM_MAINTAINER_MODE
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])


AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AS
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

#Initialize target options
TARGET_IS_9615=no

# Checks for libraries.
PKG_CHECK_MODULES([DIAG], [diag])
AC_SUBST([DIAG_CFLAGS])
AC_SUBST([DIAG_LIBS])

PKG_CHECK_MODULES([QMI], [qmi])
AC_SUBST([QMI_CFLAGS])
AC_SUBST([QMI_LIBS])

PKG_CHECK_MODULES([QMIFRAMEWORK], [qmi-framework])
AC_SUBST([QMIFRAMEWORK_CFLAGS])
AC_SUBST([QMIFRAMEWORK_LIBS])

PKG_CHECK_MODULES([QMICLIENTHELPER], [qmi-client-helper])
AC_SUBST([QMICLIENTHELPER_CFLAGS])
AC_SUBST([QMICLIENTHELPER_LIBS])

PKG_CHECK_MODULES([DSUTILS], [dsutils])
AC_SUBST([DSUTILS_CFLAGS])
AC_SUBST([DSUTILS_LIBS])

PKG_CHECK_MODULES([XMLLIB], [xmllib])
AC_SUBST([XMLLIB_CFLAGS])
AC_SUBST([XMLLIB_LIBS])

PKG_CHECK_MODULES([DATA], [data])
AC_SUBST([DATA_CFLAGS])
AC_SUBST([DATA_LIBS])

PKG_CHECK_MODULES([audcal], [audcal], [HAVE_AUDIO=1], [HAVE_AUDIO=0])
AM_CONDITIONAL([USE_AUDIO], [test "$HAVE_AUDIO" -eq 1])

AS_CASE([$host],
    [arm*], [ARM=yes],
    [ARM=no]
)

AC_ARG_WITH([glib],
      AC_HELP_STRING([--with-glib],
         [enable glib, building HLOS systems which use glib]))

if (test "x${with_glib}" = "xyes"); then
        AC_DEFINE(ENABLE_USEGLIB, 1, [Define if HLOS systems uses glib])
        PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= 2.16, dummy=yes,
                                AC_MSG_ERROR(GThread >= 2.16 is required))
        PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.16, dummy=yes,
                                AC_MSG_ERROR(GLib >= 2.16 is required))
        GLIB_CFLAGS="$GLIB_CFLAGS $GTHREAD_CFLAGS"
        GLIB_LIBS="$GLIB_LIBS $GTHREAD_LIBS"

        AC_SUBST(GLIB_CFLAGS)
        AC_SUBST(GLIB_LIBS)
fi

AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

AC_ARG_WITH([common_includes],
      AC_HELP_STRING([--with-common-includes=@<:@dir@:>@],
         [Specify the location of the common headers]),
      [common_incdir=$withval],
      with_common_includes=no)

if test "x$with_common_includes" != "xno"; then
   CPPFLAGS="${CPPFLAGS} -I${common_incdir}"
fi

AC_ARG_ENABLE(target,
	[AS_HELP_STRING([--enable-target=TARGET], [Specify the target product to build])],
	[TARGET=$enableval],
	[TARGET=none]
)
if test "x$TARGET" = "x9615-cdp"; then
	TARGET_IS_9615=yes
	AC_MSG_NOTICE([Target is 9615.])
fi

AM_CONDITIONAL([TARGET_IS_9615], [test "x$TARGET_IS_9615" = "xyes"])

AM_CONDITIONAL(ARM, [test "x$ARM" = "xyes"])
AC_SUBST([CPPFLAGS])
AC_SUBST([CC])
AC_CONFIG_FILES([ \
        Makefile \
        ipc/Makefile \
        mcm_log_util/Makefile \
        mcm_ssr_util/Makefile \
        client/Makefile \
        server/Makefile \
        mcm_data/Makefile \
        mcm_data/tests/Makefile \
        mcm_mobileap/server/src/Makefile \
        mcm_mobileap/client/src/Makefile \
        mcm_atcop/server/src/Makefile \
        mcm_atcop/client/src/Makefile \
        mcmtest/Makefile \
        mcm_sim_test/Makefile \
        mcm-core.pc
        ])
AC_OUTPUT
