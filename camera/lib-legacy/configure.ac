#                                               -*- Autoconf -*-
# configure.ac -- Autoconf script for mm-camera-core
#

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT([Camera-Hal],1.0.0)

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
        [CPPFLAGS="$CPPFLAGS -idirafter $withval"])

AC_ARG_WITH([common_includes],
           AC_HELP_STRING([--with-common-includes=@<:@dir@:>@],
           [Specify the location of the common headers]),
           [COMMON_INCDIR=$withval],
           with_common_includes=no)

if test "x$with_common_includes" != "xno"; then
   CFLAGS="${CFLAGS} -I${COMMON_INCDIR}"
   AC_SUBST(COMMON_INCDIR)
fi


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
        CPPFLAGS="${CPPFLAGS} ${GLIB_CFLAGS}"
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
KERNEL_VER=$(PREFERRED_VERSION_linux-msm)

AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

AC_SUBST([TARGET_BOARD_PLATFORM])
AC_SUBST([KERNEL_VER])
AM_CONDITIONAL([MSM8909], [test "x$TARGET_BOARD_PLATFORM" = "x8909"])
AM_CONDITIONAL([TARGET_USES_ION], [test "x$TARGET_USES_ION" = "xtrue"])
AM_CONDITIONAL([DRONE_TARGET], [test "x$DRONE_TARGET" = "xtrue"])
AM_CONDITIONAL([TARGET_USES_MEDIA_EXTENSIONS], [test "x$TARGET_USES_MEDIA_EXTENSIONS" = "xtrue"])
AM_CONDITIONAL([TARGET_TS_MAKEUP], [test "x$TARGET_TS_MAKEUP" = "xtrue"])
AM_CONDITIONAL([VENUS_SUPPORTED], [test "x$VENUS_SUPPORTED" = "xtrue"])
AM_CONDITIONAL([UBWC_PRESENT], [test "x$UBWC_PRESENT" = "xtrue"])
AM_CONDITIONAL([TARGET_SUPPORTS_DUAL_JPEG], [test "x$TARGET_SUPPORTS_DUAL_JPEG" = "xtrue"])
AM_CONDITIONAL([JPEG_USE_PIPELINE], [test "x$JPEG_USE_PIPELINE" = "xtrue"])
AM_CONDITIONAL([USE_BIONIC_HEADER], [test "x$USE_BIONIC_HEADER" = "xtrue"])
AM_CONDITIONAL([TARGET_USES_AOSP], [test "x$TARGET_USES_AOSP" = "xtrue"])

AC_OUTPUT([ \
  Makefile \
  mm-image-codec/qomx_core/Makefile \
  QCamera2/stack/Makefile \
  QCamera2/stack/mm-camera-interface/Makefile \
  QCamera2/stack/mm-camera-test/Makefile \
  QCamera2/stack/mm-jpeg-interface/Makefile \
  QCamera2/stack/mm-jpeg-interface/test/Makefile \
  QCamera2/HAL/Makefile \
])
