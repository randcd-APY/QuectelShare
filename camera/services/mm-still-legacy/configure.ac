#                                               -*- Autoconf -*-
# configure.ac -- Autoconf script for mm-camera-core
#

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT([mm-still],1.0.0)

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
        [CPPFLAGS="$CPPFLAGS -idirafter $withval"])

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
fi

AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

AC_ARG_WITH([camera-hal-path],
            AS_HELP_STRING([--with-camera-hal-path=@<:@dir@:>@],[path to camera hal]),
            [CAMERAHAL_PATH="$withval"])
AC_SUBST(CAMERAHAL_PATH)

AC_ARG_WITH([mm-camera-path],
            AS_HELP_STRING([--with-mm-camera-path=@<:@dir@:>@],[path to mm-camera module ]),
            [MM_CAMERA_PATH="$withval"])
AC_SUBST(MM_CAMERA_PATH)

AC_ARG_WITH([mm-camerasdk],
            AS_HELP_STRING([--with-mm-camerasdk=@<:@dir@:>@],[path to mm-camerasdk module ]),
            [MM_CAMERASDK_PATH="$withval"])
AC_SUBST(MM_CAMERASDK_PATH)

AC_ARG_WITH([common_includes],
        AC_HELP_STRING([--with-common-includes=@<:@dir@:>@],
        [Specify the location of the common headers]),
        [COMMON_INCDIR=$withval],
        with_common_includes=no)

if test "x$with_common_includes" != "xno"; then
   CFLAGS="${CFLAGS} -I${COMMON_INCDIR}"
   AC_SUBST(COMMON_INCDIR)
fi

CPPFLAGS="${CPPFLAGS} -I ${COMMON_INCDIR}/mm_camera_interface"
CPPFLAGS="${CPPFLAGS} -Wno-error -Wno-uninitialized -Wno-error=attributes -Wno-error=unused-parameter"
CPPFLAGS="${CPPFLAGS} -Wno-error=builtin-macro-redefined -Wno-error=type-limits"
CPPFLAGS="${CPPFLAGS} -D_GNU_SOURCE"
CPPFLAGS="${CPPFLAGS} -DUNIX_PATH_MAX=108"
CPPFLAGS="${CPPFLAGS} -DIOT_PLATFORM"
CPPFLAGS="${CPPFLAGS} -I${COMMON_INCDIR}/adreno/"
CPPFLAGS="${CPPFLAGS} -D__unused=\"__attribute__((__unused__))\""


# Define below flags as needed for various targets in corresponding bitbake recipe/.inc file
AC_SUBST([CHROMATIX_VERSION])
AC_SUBST([TARGET_BOARD_PLATFORM])
AM_CONDITIONAL([MSM8953], [test "x$TARGET_BOARD_PLATFORM" = "x8909"])
AM_CONDITIONAL([TARGET_USES_ION], [test "x$TARGET_USES_ION" = "xtrue"])
AM_CONDITIONAL([NEW_LOG_API], [test "x$NEW_LOG_API" = "xtrue"])
AM_CONDITIONAL([MM_DEBUG], [test "x$MM_DEBUG" = "xtrue"])
AM_CONDITIONAL([USES_GEMINI], [test "x$USES_GEMINI" = "xtrue"])
AM_CONDITIONAL([USES_MERCURY], [test "x$USES_MERCURY" = "xtrue"])
AM_CONDITIONAL([TARGET_ENABLE_INSTRUMENTATION], [test "x$TARGET_ENABLE_INSTRUMENTATION" = "xtrue"])
AM_CONDITIONAL([BURST_LEN_16], [test "x$BURST_LEN_16" = "xtrue"])
AM_CONDITIONAL([Q5_SW], [test "x$JPEC_ENC" = "xq5_sw"])
AM_CONDITIONAL([SW_ONLY], [test "x$JPEC_ENC" = "xsw_only"])
AM_CONDITIONAL([HW_SW], [test "x$JPEC_ENC" = "xhw_sw"])
AM_CONDITIONAL([SMIPOOL_AVAILABLE], [test "x$SMIPOOL_AVAILABLE" = "xtrue"])
AM_CONDITIONAL([USES_ARMV7], [test "x$USES_ARMV7" = "xtrue"])
AM_CONDITIONAL([USE_FASTCV_OPT], [test "x$USE_FASTCV_OPT" = "xtrue"])
AM_CONDITIONAL([CODECB], [test "x$FACT_VER" = "xcodecB"])
AM_CONDITIONAL([CODECA], [test "x$FACT_VER" = "xcodecA"])
AM_CONDITIONAL([USE_PERF_LOCK], [test "x$USE_PERF_LOCK" = "xtrue"])
AM_CONDITIONAL([OMX_JPEG_PIPELINE], [test "x$OMX_JPEG_PIPELINE" = "xtrue"])

AC_OUTPUT([ \
  Makefile \
  jpeg_hw_10/jpegd_hw/Makefile \
  jpeg_hw_10/jpege_hw/Makefile \
  jpeg2/Makefile \
  ipl/Makefile \
  codec_v1/Makefile \
  codec_v1/jpeg_sw/Makefile \
  codec_v1/omx/Makefile \
  codec_v1/omx/jpeg_decoder/Makefile \
  codec_v1/omx/jpeg_encoder/Makefile
])
