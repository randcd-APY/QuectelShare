#                                               -*- Autoconf -*-

# configure.ac -- Autoconf script for qcacld-tools
#

# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the qcacld-tools package version 1.0.0
AC_INIT([qcacld-tools],1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
LT_INIT

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

AC_ARG_WITH([diag],
      AC_HELP_STRING([--without-diag],
         [Not using libdiag, third party platform does not support diag]),
         [],
         [with_diag=yes])

if (test "x${with_diag}" = "xyes"); then
        # Checks for libraries.
        PKG_CHECK_MODULES([DIAG], [diag])
        AC_SUBST([DIAG_CFLAGS])
        AC_SUBST([DIAG_LIBS])
else
        CPPFLAGS="${CPPFLAGS} -DNO_DIAG_SUPPORT"
fi

AC_ARG_WITH([glib],
      AC_HELP_STRING([--with-glib],
         [enable glib, building FTM Daemon which use glib]))

if (test "x${with_glib}" = "xyes"); then
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

AC_ARG_ENABLE([debug],
        [  --enable-debug    Turn on debugging],
        [case "${enableval}" in
            yes) debug=true ;;
            no)  debug=false ;;
            *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
        esac],[debug=false])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AC_ARG_ENABLE([target-mdm9650],
        AC_HELP_STRING([--enable-target-mdm9650],
                [Enable conditional compile for target mdm9650]),
        [target_mdm9650="${enableval}"],
        target_mdm9650=no)

AC_ARG_ENABLE([target],
	AC_HELP_STRING([--enable-target],
		[Enable conditional compile for target]),
	[TARGET="${enableval}"],
	[TARGET=no])

AM_CONDITIONAL([NEED_SCM], [test "x$TARGET" = "xsdx20" -o "x$TARGET" = "xmdm9650" -o "x$TARGET" = "xsdxpoorwills"])

AM_CONDITIONAL([THIRD_PARTY_SUPPORTED], [test "x$TARGET" = "xthirdparty"])
if test "x$TARGET" = "xthirdparty"; then
        CPPFLAGS="${CPPFLAGS} -DCNSS_DIAG_THIRD_PARTY_PLATFORM"
fi

if test "x$TARGET" = "xsdx20" -o "x$TARGET" = "xmdm9650" -o "x$TARGET" = "xsdxpoorwills"; then
        AC_ARG_WITH([qmi_cli],
                AC_HELP_STRING([--with-qmi_cli=@<:@dir@:>@],
                [Enable QMI clients]),
                [],
                with_qmi_cli=no)

        if test "x$with_qmi_cli" != "xno"; then
                PKG_CHECK_MODULES([QMI], [qmi])
                AC_SUBST([QMI_CFLAGS])
                AC_SUBST([QMI_LIBS])

                PKG_CHECK_MODULES([QMI_FRAMEWORK], [qmi-framework])
                AC_SUBST([QMI_FRAMEWORK_CFLAGS])
                AC_SUBST([QMI_FRAMEWORK_LIBS])
        fi
        AM_CONDITIONAL([QMI_CLI], [test x$with_qmi_cli != xno])

        has_libnl_ver=0
        # libnl-2 provides only libnl-2.0.pc file, so we check for separate libnl-genl-3.0.pc
        # pkg-config file just for libnl-3.0 case.
        #
        PKG_CHECK_MODULES([LIBNL], [libnl-3.0 >= 3.0 libnl-genl-3.0 >= 3.0], [has_libnl_ver=3], [
           PKG_CHECK_MODULES([LIBNL], [libnl-2.0 >= 2.0], [has_libnl_ver=2], [
                   PKG_CHECK_MODULES([LIBNL], [libnl-1], [has_libnl_ver=1], [has_libnl_ver=0])])])

        if (test "$has_libnl_ver" -eq 0); then
                AC_MSG_ERROR(libnl and libnl-genl are required but were not found)
        fi

        if (test "$has_libnl_ver" -gt 1); then
                AC_DEFINE([HAVE_LIBNL20], [1], [Define if you have libnl-2.0 or higher])
        fi

        if (test "$has_libnl_ver" -gt 2); then
                AC_DEFINE([HAVE_LIBNL30], [1], [Define if you have libnl-3.0 or higher])
        fi

        AC_SUBST([LIBNL_CFLAGS])
        AC_SUBST([LIBNL_LIBS])

        AC_ARG_WITH([qxdm],
                AC_HELP_STRING([--with-qxdm=@<:@dir@:>@],
                        [Enable qxdm logging]),
                        [],
                        with_qxdm=no)

        if test "x$with_qxdm" != "xno"; then
                CPPFLAGS="${CPPFLAGS} -DFEATURE_DATA_LOG_QXDM"
                PKG_CHECK_MODULES([DSUTILS], [dsutils])
                AC_SUBST([DSUTILS_CFLAGS])
                AC_SUBST([DSUTILS_LIBS])
        fi
else
        has_libnl_ver=0
        # libnl-2 provides only libnl-2.0.pc file, so we check for separate libnl-genl-3.0.pc
        # pkg-config file just for libnl-3.0 case.
        #
        PKG_CHECK_MODULES([LIBNL], [libnl-3.0 >= 3.0 libnl-genl-3.0 >= 3.0], [has_libnl_ver=3], [
           PKG_CHECK_MODULES([LIBNL], [libnl-2.0 >= 2.0], [has_libnl_ver=2], [
                   PKG_CHECK_MODULES([LIBNL], [libnl-1], [has_libnl_ver=1], [has_libnl_ver=0])])])

        if (test "$has_libnl_ver" -eq 0); then
                AC_MSG_ERROR(libnl and libnl-genl are required but were not found)
        fi

        if (test "$has_libnl_ver" -gt 1); then
                AC_DEFINE([HAVE_LIBNL20], [1], [Define if you have libnl-2.0 or higher])
        fi

        if (test "$has_libnl_ver" -gt 2); then
                AC_DEFINE([HAVE_LIBNL30], [1], [Define if you have libnl-3.0 or higher])
        fi

        AC_SUBST([LIBNL_CFLAGS])
        AC_SUBST([LIBNL_LIBS])

	AM_CONDITIONAL([QMI_CLI], 0)
fi

AC_ARG_WITH([cld80211],
      AC_HELP_STRING([--with-cld80211],
         [enable cld80211, building spectraltool which uses cld80211]))

if (test "x${with_cld80211}" = "xyes"); then
	PKG_CHECK_MODULES([CLD80211_LIB], [cld80211-lib])
	AC_SUBST([CLD80211_LIB_CFLAGS])
	AC_SUBST([CLD80211_LIB_LIBS])
fi
AM_CONDITIONAL([NEED_SPECTRAL], test "x${with_cld80211}" = "xyes")

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_HEADER_STDC
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC

AC_CONFIG_FILES([ \
		Makefile \
		cnssdiag/Makefile \
		athdiag/Makefile \
		pktlog/Makefile \
		wlan_scm/Makefile \
		asf/Makefile \
		asf/AniAsf.pc \
		ptt/Makefile \
		csm/Makefile \
		spectral/Makefile \
        ])
AC_OUTPUT

