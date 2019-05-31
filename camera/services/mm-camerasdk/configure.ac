#                                               -*- Autoconf -*-
# configure.ac -- Autoconf script for mm-camera-core
#

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT([mm-camera],1.0.0)

AM_INIT_AUTOMAKE([-Wall -Werror gnu foreign subdir-objects])
AM_MAINTAINER_MODE

AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AM_PROG_AR
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_AS

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

AC_CHECK_TOOL(OBJCOPY, objcopy, false)

AC_ARG_WITH(sanitized-headers,
	    AS_HELP_STRING([--with-sanitized-headers=DIR],[Specify the location of the sanitized Linux headers]),
	    [CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH([glib],
            AC_HELP_STRING([--with-glib],
            [enable glib, Build against glib. Use this when building for HLOS systems which use glib]))

if (test "x${with_glib}" = "xyes"); then
        PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= 2.16, dummy=yes,
                                AC_MSG_ERROR(GThread >= 2.16 is required))
        PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.16, dummy=yes,
				AC_MSG_ERROR(GLib >= 2.16 is required))
	GLIB_CFLAGS="$GLIB_CFLAGS $GTHREAD_CFLAGS"
        GLIB_LIBS="$GLIB_LIBS $GTHREAD_LIBS"

        AC_SUBST(GLIB_CFLAGS)
        AC_SUBST(GLIB_LIBS)
	LDFLAGS="${LDFLAGS} ${GLIB_LIBS}"
	CPPFLAGS="${CPPFLAGS} ${GLIB_CFLAGS}"
fi

AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

AC_ARG_WITH([common_includes],
            AC_HELP_STRING([--with-common-includes=@<:@dir@:>@],
            [Specify the location of the common headers]),
            [COMMON_INCDIR=$withval],
            with_common_includes=no)

if test "x$with_common_includes" != "xno"; then
   CFLAGS="${CFLAGS} -I${COMMON_INCDIR}"
   AC_SUBST(COMMON_INCDIR)
fi

CXXFLAGS="${CXXFLAGS} -std=c++11 -std=gnu++0x"

CFLAGS="${CFLAGS} -Wno-error -Wno-uninitialized -Wno-error=attributes -Wno-error=unused-parameter"
CFLAGS="${CFLAGS} -Wno-error=builtin-macro-redefined -Wno-error=type-limits"

CXXFLAGS="${CXXFLAGS} -Wno-error -Wno-uninitialized -Wno-error=attributes -Wno-error=unused-parameter"
CXXFLAGS="${CXXFLAGS} -Wno-error=builtin-macro-redefined -Wno-error=type-limits"

CPPFLAGS="${CPPFLAGS} -D_GNU_SOURCE"
CPPFLAGS="${CPPFLAGS} -D_LE_CAMERA_"
CPPFLAGS="${CPPFLAGS} -DUNIX_PATH_MAX=108"
CPPFLAGS="${CPPFLAGS} -Dstrlcpy=g_strlcpy"
CPPFLAGS="${CPPFLAGS} -Dstrlcat=g_strlcat"
CPPFLAGS="${CPPFLAGS} -include stdint.h"
CPPFLAGS="${CPPFLAGS} -include cutils/trace.h"
CPPFLAGS="${CPPFLAGS} -include sys/ioctl.h"
CPPFLAGS="${CPPFLAGS} -include glib.h"
CPPFLAGS="${CPPFLAGS} -D__unused=\"__attribute__((__unused__))\""


AC_SUBST([TARGET_BOARD_PLATFORM])
AC_SUBST([CHROMATIX_VERSION])

AM_CONDITIONAL([MSM8953], [test "x$TARGET_BOARD_PLATFORM" = "x8953"])
AM_CONDITIONAL([MSM8996], [test "x$TARGET_BOARD_PLATFORM" = "x8996"])
AM_CONDITIONAL([MSM8998], [test "x$TARGET_BOARD_PLATFORM" = "x8998"])

MMCAMERA_DEBUG_DEFINES="-D_ANDROID_"
CPPFLAGS="${CPPFLAGS} ${MMCAMERA_DEBUG_DEFINES}"

AC_OUTPUT([ \
  Makefile \
  hvx/Makefile 
])
