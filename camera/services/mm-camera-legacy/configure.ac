AC_PREREQ(2.69)

AC_INIT([mm-camera-legacy],1.0.0)

AM_INIT_AUTOMAKE([-Werror -Wall gnu foreign subdir-objects])

AM_MAINTAINER_MODE

AC_CONFIG_HEADER([configure.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AM_PROG_AS
AM_PROG_AR
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
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

COMPILE_CAMERA=yes
AMSS_VERSION=
VFE_VERS=
MSM_VERSION=
DEBUG_CPPFLAGS=
DEBUG_CFLAGS=
DEBUG_LDFLAGS=
KERNEL_VER=$(PREFERRED_VERSION_linux-msm)

AC_ARG_WITH([dlog],
      AC_HELP_STRING([--with-dlog],
         [enable dlog, building HLOS systems which use dlog]))

if (test "x${with_dlog}" = "xyes"); then
      AC_DEFINE(ENABLE_USEDLOG, 1, [Define if HLOS systems uses dlog])
      PKG_CHECK_MODULES(DLOG, [dlog])
      AC_SUBST(DLOG_CFLAGS)
      AC_SUBST(DLOG_LIBS)
fi

AM_CONDITIONAL(USE_DLOG, test "x${with_dlog}" = "xyes")

AC_ARG_ENABLE([mvdg],
       [AS_HELP_STRING([--enable-mvdg=MVDG_PRESENT],[Build mvdg module])],
       [],
       [enable-mvdg='False']
)

AC_ARG_WITH([sanitized-headers],
       [AS_HELP_STRING([--with-sanitized-headers=DIR],[location of the sanitized Linux kernel headers])],
       [CFLAGS="$CFLAGS -I$withval/media -I $withval"],
       [CPPFLAGS="$CPPFLAGS -I$withval/media -I$withval"],
       [CXXFLAGS="$CXXFLAGS -I$withval/media -I$withval"])

AC_ARG_WITH([additional-include-directives],
       [AS_HELP_STRING([--with-additional-include-directives=DIR],
       [additional -I include paths for the C pre-processor])],
       [CPPFLAGS="$CPPFLAGS -I$withval"],
       [CXXFLAGS="$CXXFLAGS -I$withval"])

AC_ARG_WITH([camera-hal-headers],
       [AS_HELP_STRING([--with-camera-hal-headers=DIR],[location of the camera HAL headers])],
       [CFLAGS="$CFLAGS -I$withval"],
       [CPPFLAGS="$CPPFLAGS -I$withval"],
       [CXXFLAGS="$CXXFLAGS -I$withval"])


AC_ARG_WITH([sensors-headers],
       [AS_HELP_STRING([--with-sensors-headers=DIR],[location of sensors API headers])],
       [CFLAGS="$CFLAGS -I$withval"],
       [CPPFLAGS="$CPPFLAGS -I$withval"],
       [CXXFLAGS="$CXXFLAGS -I$withval"])

AC_ARG_WITH([adreno200-headers],
       [AS_HELP_STRING([--with-adreno200-headers=DIR],[location of adreno/C2D headers])],
       [CFLAGS="$CFLAGS -I$withval"],
       [CPPFLAGS="$CPPFLAGS -I$withval"],
       [CXXFLAGS="$CXXFLAGS -I$withval"])

AC_ARG_WITH([qmi-framework-headers],
       [AS_HELP_STRING([--with-qmi-framework-headers=DIR],[location of qmi-framework headers])],
       [CFLAGS="$CFLAGS -I$withval"],
       [CPPFLAGS="$CPPFLAGS -I$withval"],
       [CXXFLAGS="$CXXFLAGS -I$withval"])

AC_ARG_WITH([extra-cflags],
       [AS_HELP_STRING([--with-extra-cflags=DIR],[additional compliation flags])],
       [CFLAGS="$CFLAGS -I$withval"],
       [CPPFLAGS="$CPPFLAGS -I$withval"],
       [CXXFLAGS="$CXXFLAGS -I$withval"])

AC_ARG_ENABLE([target],
       [AS_HELP_STRING([--enable-target=TARGET],[target to build for])],
       [],
       [enable_target=none]
)

if test "$enable_mvdg" = "True"; then
    AC_ARG_WITH([mvdg-headers],
        [AS_HELP_STRING([--with-mvdg-headerss=DIR],[location of mvdg lib headers])],
           [CFLAGS="$CFLAGS -I$withval"],
           [CPPFLAGS="$CPPFLAGS -I$withval"],
           [CXXFLAGS="$CXXFLAGS -I$withval"])
    FEATURE_MVDG_ENABLED=true
else
    FEATURE_MVDG_ENABLED=false
fi

if test "x$enable_target" = "xmsm8909"; then
       VFE_VERS=vfe32
       MSM_VERSION=8909
       #FEATURE_WAVELET_DENOISE=true
       FEATURE_ZSL=true
       FEATURE_FACE_PROC=true
       FEATURE_VFE_TEST_VEC=false
        USE_CAC_V1=false
       #DEBUG_CPPFLAGS=-DHW_ENCODE -DCONFIG_MSG_THESHOLD=350
       #DEBUG_CFLAGS=-DHW_ENCODE -DCONFIG_MSG_THESHOLD=350
else
       AC_MSG_NOTICE([A valid target was not specified. Please check the '--enable-target' option.])
       COMPILE_CAMERA=no
fi

if test "x$VFE_VERS" = "xvfe32"; then
       CHROMATIX_VERSION=0208
elif test "x$VFE_VERS" = "xvfe31"; then
       CHROMATIX_VERSION=0208
elif test "x$VFE_VERS" = "xvfe2x"; then
       CHROMATIX_VERSION=0208
elif test "x$VFE_VERS" = "xvfe32"; then
       CHROMATIX_VERSION=0301
else
       CHROMATIX_VERSION=0205
fi

AC_ARG_ENABLE(amss-version,
       [AS_HELP_STRING([--enable-amss-version=AMSS_VERSION],[Specify the AMSS version])],
       [AMSS_VERSION=$enableval],
       [AMSS_VERSION=]
)
AC_SUBST([AMSS_VERSION])

AC_ARG_ENABLE([debug],
       [AS_HELP_STRING([--enable-debug],[Build with debug flags and options])],
       [DEBUG=$enableval],
       [DEBUG=no]
)
if test "x$DEBUG" = "xyes"; then
       DEBUG_CPPFLAGS="${DEBUG_CPPFLAGS} -DLOG_DEBUG -DLOG_TAG=\"CameraService\""
       DEBUG_CFLAGS="${DEBUG_CFLAGS} -DLOG_DEBUG -DLOG_TAG=\"CameraService\" -g -O0"
fi

AM_CONDITIONAL([COMPILE_CAMERA], [test "x$COMPILE_CAMERA" = "xyes"])
AC_SUBST([VFE_VERS])
AC_SUBST([MSM_VERSION])
AC_SUBST([CHROMATIX_VERSION])
AC_SUBST([DEBUG_CPPFLAGS])
AC_SUBST([DEBUG_CFLAGS])
AC_SUBST([DEBUG_LDFLAGS])
AC_SUBST([FEATURE_FACE_PROC])
AC_SUBST([FEATURE_WAVELET_DENOISE])
AC_SUBST([FEATURE_VFE_TEST_VEC])
AC_SUBST([FEATURE_MVDG_ENABLED])

AC_SUBST([KERNEL_VER])
if test "x$enable_target" = "xmsm8909"; then
FEATURE_GYRO=false
else
FEATURE_GYRO=false
fi
FEATURE_SENSOR_HAL=false
#FEATURE_SENSOR_IMU=true
FEATURE_GYRO_DUMP_DATA=false
FEATURE_CPP_IMU=false

AM_CONDITIONAL([FEATURE_GYRO], [test "x$FEATURE_GYRO" = "xfalse"])
AM_CONDITIONAL([FEATURE_SENSOR_HAL], [test "x$FEATURE_SENSOR_HAL" = "xtrue"])
AM_CONDITIONAL([FEATURE_SENSOR_IMU], [test "x$FEATURE_SENSOR_IMU" = "xtrue"])
AM_CONDITIONAL([FEATURE_GYRO_DUMP_DATA], [test "x$FEATURE_GYRO_DUMP_DATA" = "xtrue"])
AM_CONDITIONAL([VFE2X], [test "x$VFE_VERS" = "xvfe2x"])
AM_CONDITIONAL([VFE30], [test "x$VFE_VERS" = "xvfe30"])
AM_CONDITIONAL([VFE31], [test "x$VFE_VERS" = "xvfe31"])
AM_CONDITIONAL([VFE32], [test "x$VFE_VERS" = "xvfe32"])
AM_CONDITIONAL([CHROMATIX0207], [test "x$CHROMATIX_VERSION" = "x0207"])
AM_CONDITIONAL([CHROMATIX0301], [test "x$CHROMATIX_VERSION" = "0301"])
AM_CONDITIONAL([WAVELET_DENOISE], [test "x$FEATURE_WAVELET_DENOISE" = "xtrue"])
AM_CONDITIONAL([VFE_TEST_VEC], [test "x$FEATURE_VFE_TEST_VEC" = "xtrue"])
AM_CONDITIONAL([FACE_PROC], [test "x$FEATURE_FACE_PROC" = "xtrue"])
AM_CONDITIONAL([USE_CAC_V1], [test "x$USE_CAC_V1" = "xtrue"])

AM_CONDITIONAL([MSM8909], [test "x$MSM_VERSION" = "x8909"])
AM_CONDITIONAL([USE_MVDG], [test "x$FEATURE_MVDG_ENABLED" = "xtrue"])
AM_CONDITIONAL([CPP_IMU], [test "x$FEATURE_CPP_IMU" = "xtrue"])

AS_CASE([$host],
       [arm*], [ARM=yes],
       [ARM=no]
)
AM_CONDITIONAL([ARM], [test "x$ARM" = "xyes"])


AC_OUTPUT([ \
  Makefile \
  mm-camera2/media-controller/Makefile \
  mm-camera2/media-controller/mct/Makefile \
  mm-camera2/media-controller/modules/iface/Makefile \
  mm-camera2/media-controller/modules/imglib/Makefile \
  mm-camera2/media-controller/modules/isp/Makefile \
  mm-camera2/media-controller/modules/pproc-new/Makefile \
  mm-camera2/media-controller/modules/pproc-new/c2d/Makefile \
  mm-camera2/media-controller/modules/pproc-new/cpp/Makefile \
  mm-camera2/media-controller/modules/pproc-new/vpe/Makefile \
  mm-camera2/media-controller/modules/pproc-new/wnr/Makefile \
  mm-camera2/media-controller/modules/sensors/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_imx241/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_imx241/common/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_imx241/preview/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_imx241/snapshot/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_imx241/video/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_ov7251/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_ov7251/common/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_ov7251/preview/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_ov7251/snapshot/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_ov7251/video/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_hi843_8909/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_hi843_8909/common/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_hi843_8909/preview/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_hi843_8909/snapshot/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_hi843_8909/video/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_hi259/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_hi259/common/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_hi259/preview/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_hi259/snapshot/Makefile \
  mm-camera2/media-controller/modules/sensors/chromatix/0301/libchromatix/chromatix_hi259/video/Makefile \
  mm-camera2/media-controller/modules/sensors/sensor_libs/Makefile \
  mm-camera2/media-controller/modules/sensors/sensor_libs/imx241/Makefile \
  mm-camera2/media-controller/modules/sensors/sensor_libs/ov7251/Makefile \
  mm-camera2/media-controller/modules/sensors/sensor_libs/ov5640/Makefile \
  mm-camera2/media-controller/modules/sensors/sensor_libs/hi843_8909/Makefile \
  mm-camera2/media-controller/modules/sensors/sensor_libs/hi259/Makefile \
  mm-camera2/media-controller/modules/stats/Makefile \
  mm-camera2/server-imaging/Makefile \
  mm-camera2/server-tuning/Makefile \
  mm-camera2/server-tuning/tuning/Makefile
])
