#                                               -*- Autoconf -*-
# configure.ac -- Autoconf script for mm-camera-core
#

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT([mm-camera],1.0.0)

AM_INIT_AUTOMAKE([-Wall gnu foreign subdir-objects])
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

AC_ARG_WITH([xml2_includes],
            AS_HELP_STRING([--with-xml2-includes=@<:@dir@:>@],[path to camera hal]),
            [SRC_XML_INCLUDE="$withval"])

AC_SUBST(SRC_XML_INCLUDE)

AC_ARG_WITH([common_includes],
            AC_HELP_STRING([--with-common-includes=@<:@dir@:>@],
            [Specify the location of the common headers]),
            [COMMON_INCDIR=$withval],
            with_common_includes=no)

if test "x$with_common_includes" != "xno"; then
   CFLAGS="${CFLAGS} -I${COMMON_INCDIR}"
   AC_SUBST(COMMON_INCDIR)
fi

AC_ARG_WITH([camera-hal-path],
            AS_HELP_STRING([--with-camera-hal-path=@<:@dir@:>@],[path to camera hal]),
            [CAMERAHAL_PATH="$withval"])

AC_SUBST(CAMERAHAL_PATH)

AC_ARG_WITH([mm-camerasdk-path],
           AS_HELP_STRING([--with-mm-camerasdk-path=@<:@dir@:>@],
           [path to mm-camerasdk module ]),
           [MM_CAMERASDK_PATH="$withval"])

AC_SUBST(MM_CAMERASDK_PATH)

AC_ARG_WITH([mm-camera-lib-path],
           AS_HELP_STRING([--with-mm-camera-lib-path=@<:@dir@:>@],
           [path to mm-camera-lib module ]),
           [MM_CAMERA_LIB_PATH="$withval"])

AC_SUBST(MM_CAMERA_LIB_PATH)

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
CPPFLAGS="${CPPFLAGS} -I ${MM_CAMERASDK_PATH}/sensor/includes/${CHROMATIX_VERSION}"

AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")
AM_CONDITIONAL([DISABLE_FASTCV], [test "x$DISABLE_FASTCV" = "xtrue"])

AC_SUBST([TARGET_BOARD_PLATFORM])
AC_SUBST([CHROMATIX_VERSION])

AM_CONDITIONAL([ENABLE_msm8953], [test "x$TARGET_BOARD_PLATFORM" = "xmsm8953"])
AM_CONDITIONAL([ENABLE_msm8996], [test "x$TARGET_BOARD_PLATFORM" = "xmsm8996"])
AM_CONDITIONAL([ENABLE_msm8992], [test "x$TARGET_BOARD_PLATFORM" = "xmsm8992"])
AM_CONDITIONAL([ENABLE_msm8084], [test "x$TARGET_BOARD_PLATFORM" = "xapq8084"])
AM_CONDITIONAL([ENABLE_msm8952], [test "x$TARGET_BOARD_PLATFORM" = "xmsm8952"])
AM_CONDITIONAL([ENABLE_msm8976], [test "x$TARGET_BOARD_PLATFORM" = "xmsm8976"])
AM_CONDITIONAL([ENABLE_msm8992], [test "x$TARGET_BOARD_PLATFORM" = "xmsm8992"])
AM_CONDITIONAL([ENABLE_msm8916], [test "x$TARGET_BOARD_PLATFORM" = "xmsm8916"])
AM_CONDITIONAL([ENABLE_msm8974], [test "x$TARGET_BOARD_PLATFORM" = "xmsm8974"])
AM_CONDITIONAL([ENABLE_msm8937], [test "x$TARGET_BOARD_PLATFORM" = "xmsm8937"])
AM_CONDITIONAL([ENABLE_apq8084], [test "x$TARGET_BOARD_PLATFORM" = "xapq8084"])
AM_CONDITIONAL([ENABLE_msmcobalt], [test "x$TARGET_BOARD_PLATFORM" = "xmsmcobalt"])
AM_CONDITIONAL([ENABLE_sdm660], [test "x$TARGET_BOARD_PLATFORM" = "xsdm660"])
AM_CONDITIONAL([ENABLE_msm8998], [test "x$TARGET_BOARD_PLATFORM" = "xmsm8998"])
AM_CONDITIONAL([ENABLE_msm8994], [test "x$TARGET_BOARD_PLATFORM" = "xmsm8994"])
AM_CONDITIONAL([TARGET_USES_ION], [test "x$TARGET_USES_ION" = "xtrue"])
AM_CONDITIONAL([DRONE_TARGET], [test "x$DRONE_TARGET" = "xtrue"])
AM_CONDITIONAL([TARGET_USES_MEDIA_EXTENSIONS], [test "x$TARGET_USES_MEDIA_EXTENSIONS" = "xtrue"])
AM_CONDITIONAL([TARGET_COMPILE_WITH_MSM_KERNEL], [test "x$TARGET_COMPILE_WITH_MSM_KERNEL" = "xtrue"])
AM_CONDITIONAL([TARGET_TS_MAKEUP], [test "x$TARGET_TS_MAKEUP" = "xtrue"])
AM_CONDITIONAL([TARGET_IS_HEADLESS], [test "x$TARGET_IS_HEADLESS" = "xtrue"])
AM_CONDITIONAL([TARGET_USES_AOSP], [test "x$TARGET_USES_AOSP" = "xtrue"])
AM_CONDITIONAL([IS_ENABLE], [test "x$IS_ENABLE" = "xtrue"])
AM_CONDITIONAL([ENABLE_C2D], [test "x$ENABLE_C2D" = "xtrue"])
AM_CONDITIONAL([USE_GYRO], [test "x$USE_GYRO" = "xtrue"])
AM_CONDITIONAL([FEATURE_GYRO_IMU], [test "x$FEATURE_GYRO_IMU" = "xtrue"])
AM_CONDITIONAL([FEATURE_DG], [test "x$FEATURE_DG" = "xtrue"])
AM_CONDITIONAL([IS_CHROMATIX_VERSION_0309], [test "x$CHROMATIX_VERSION" = "x0309"])
AM_CONDITIONAL([IS_CHROMATIX_VERSION_0310], [test "x$CHROMATIX_VERSION" = "x0310"])
AM_CONDITIONAL([IS_CHROMATIX_VERSION_0308E], [test "x$CHROMATIX_VERSION" = "x0308E"])
AM_CONDITIONAL([FEATURE_SVHDR_EN], [test "x$FEATURE_SVHDR_EN" = "xtrue"])
AM_CONDITIONAL([FEATURE_BINCORR], [test "x$FEATURE_BINCORR" = "xtrue"])
AM_CONDITIONAL([FEATURE_CAC], [test "x$FEATURE_CAC" = "xtrue"])
AM_CONDITIONAL([FEATURE_SW_TNR], [test "x$FEATURE_SW_TNR" = "xtrue"])
AM_CONDITIONAL([ENABLE_MM_DEBUG], [test "x$MM_DEBUG" = "xtrue"])
AM_CONDITIONAL([JPEG_DMA], [test "x$JPEG_DMA" = "xtrue"])
AM_CONDITIONAL([FD_USES_DSP], [test "x$FD_USES_DSP" = "xtrue"])
AM_CONDITIONAL([USE_CDSP], [test "x$USE_CDSP" = "xtrue"])
AM_CONDITIONAL([FD_ENABLE_SW_INTERMITTENT], [test "x$FD_ENABLE_SW_INTERMITTENT" = "xtrue"])
AM_CONDITIONAL([FD_USE_V65], [test "x$FD_USE_V65" = "xtrue"])
AM_CONDITIONAL([IMGLIB_USES_HVX], [test "x$IMGLIB_USES_HVX" = "xtrue"])
AM_CONDITIONAL([IMGLIB_USES_FASTCV], [test "x$IMGLIB_USES_FASTCV" = "xtrue"])
AM_CONDITIONAL([FACEPROC_USE_NEW_WRAPPER], [test "x$FACEPROC_USE_NEW_WRAPPER" = "xtrue"])
AM_CONDITIONAL([USE_CAC_V1], [test "x$USE_CAC_V1" = "xtrue"])
AM_CONDITIONAL([USE_CAC_V2], [test "x$USE_CAC_V2" = "xtrue"])
AM_CONDITIONAL([USE_CAC_V3], [test "x$USE_CAC_V3" = "xtrue"])
AM_CONDITIONAL([PROFILE_FD], [test "x$PROFILE_FD" = "xtrue"])
AM_CONDITIONAL([USE_FASTCV_OPT], [test "x$USE_FASTCV_OPT" = "xtrue"])
AM_CONDITIONAL([BOARD_QTI_CAMERA_V2], [test "x$BOARD_QTI_CAMERA_V2" = "xtrue"])
AM_CONDITIONAL([CAMERA_USE_CHROMATIX_HW_WNR_TYPE], [test "x$CAMERA_USE_CHROMATIX_HW_WNR_TYPE" = "xtrue"])
AM_CONDITIONAL([ENABLED_IMX477_RAW10_MODES], [test "x$ENABLED_IMX477_RAW10_MODES"  = "xtrue"])
AM_CONDITIONAL([ENABLE_BOARD_QTI_CAMERA_V2], [test x$ENABLE_BOARD_QTI_CAMERA_V2  = xyes])
AM_CONDITIONAL([ENABLE_OEM_CHROMATIX], [test x$ENABLE_OEM_CHROMATIX = xyes])
AM_CONDITIONAL([DISABLE_ADRENO_GPU], [test "x$DISABLE_ADRENO_GPU" = "xtrue"])
AM_CONDITIONAL([CAMERA_DEBUG_DATA], [test "x$CAMERA_DEBUG_DATA" = "xtrue"])

CPPFLAGS="${CPPFLAGS} -D_ANDROID_"

if (test "x$DRONE_TARGET" = "xtrue"); then
CPPFLAGS="${CPPFLAGS} -D_DRONE_"
fi

if (test "x$CAMERA_DEBUG_DATA" = "xtrue"); then
CPPFLAGS="${CPPFLAGS} -DCAMERA_DEBUG_DATA"
fi

if (test "x$MM_DEBUG" = "xtrue"); then
CPPFLAGS="${CPPFLAGS} -DLOG_DEBUG"
CFLAGS="${CFLAGS} -g -O0"
LDFLAGS="${LDFLAGS} -llog"
fi

CPPFLAGS="${CPPFLAGS} ${DEBUG_CPPFLAGS}"

if (test "x$USE_GYRO" = "xtrue"); then
	FEATURE_GYRO_DSPS=true
else
	FEATURE_GYRO_DSPS=false
fi
if (test "x$FEATURE_GYRO_IMU" = "xtrue"); then
	FEATURE_GYRO_DSPS=false
fi

AM_CONDITIONAL([FEATURE_GYRO_DSPS], [test "x$FEATURE_GYRO_DSPS" = "xtrue"])

FDDSP_USE_SW_WRAPPER_FOR_ALL=false
if (test "x$FACEPROC_USE_NEW_WRAPPER" = "xtrue"); then
	if (test "x$FD_USE_V65" = "xtrue"); then
		if (test "x$FD_USES_DSP" = "xtrue"); then
            		FDDSP_USE_SW_WRAPPER_FOR_ALL=true
		fi
	fi
fi
AM_CONDITIONAL([FDDSP_USE_SW_WRAPPER_FOR_ALL], [test "x$FDDSP_USE_SW_WRAPPER_FOR_ALL" = "xtrue"])

AC_OUTPUT([ \
  Makefile \
  media-controller/Makefile \
  services/Makefile \
  services/tests/Makefile \
  services/thread/Makefile \
  server-imaging/Makefile \
  server-tuning/Makefile \
  server-tuning/lookup/Makefile \
  server-tuning/tuning/Makefile \
  media-controller/mct/Makefile \
  media-controller/mct_shim_layer/Makefile \
  media-controller/modules/Makefile \
  media-controller/modules/isp2/Makefile \
  media-controller/modules/isp2/module/Makefile \
  media-controller/modules/iface2/Makefile \
  media-controller/modules/stats/Makefile \
  media-controller/modules/imglib/Makefile \
  media-controller/modules/pproc-new/Makefile \
  media-controller/modules/pproc-new/buf_mgr/Makefile \
  media-controller/modules/pproc-new/cpp/Makefile \
  media-controller/modules/pproc-new/c2d/Makefile \
  media-controller/modules/pproc-new/base/Makefile \
  media-controller/modules/pproc-new/eztune/Makefile \
  media-controller/modules/sensors/Makefile \
  media-controller/modules/sensors/ois/libs/bu63165/Makefile \
  media-controller/modules/sensors/ois/libs/bu63164/Makefile \
  media-controller/modules/sensors/ois/libs/lc898122/Makefile \
  media-controller/modules/sensors/ois/libs/Makefile \
  media-controller/modules/sensors/eebin/libs/Makefile \
  media-controller/modules/sensors/sensor/libs/Makefile \
  media-controller/modules/sensors/sensor/libs/imx230/Makefile \
  media-controller/modules/sensors/sensor/libs/imx274/Makefile \
  media-controller/modules/sensors/sensor/libs/imx378/Makefile \
  media-controller/modules/sensors/sensor/libs/imx477/Makefile \
  media-controller/modules/sensors/sensor/libs/imx377/Makefile \
  media-controller/modules/sensors/sensor/libs/ov7251/Makefile \
  media-controller/modules/sensors/sensor/libs/ar1335/Makefile \
  media-controller/modules/sensors/sensor/libs/ov9281/Makefile \
  media-controller/modules/sensors/chromatix/0309/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx230/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx230/3A/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx230/common/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx230/cpp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx230/postproc/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx230/isp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx230_qc2002_with_gyro/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx230_qc2002_with_gyro/3A/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx230_qc2002_with_gyro/common/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx230_qc2002_with_gyro/cpp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx230_qc2002_with_gyro/postproc/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx230_qc2002_with_gyro/isp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx274/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx274/3A/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx274/iot/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx274/cpp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx274/common/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx274/postproc/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx274/isp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx378/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx378/3A/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx378/common/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx378/cpp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx378/iot/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx378/isp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx378/postproc/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx477/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx477/3A/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx477/common/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx477/cpp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx477/iot/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx477/isp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx477/postproc/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ar1335/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ar1335/3A/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ar1335/iot/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ar1335/cpp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ar1335/common/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ar1335/postproc/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ar1335/isp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx377/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx377/3A/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx377/common/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx377/cpp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx377/iot/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx377/isp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_imx377/postproc/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ov7251/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ov7251/3A/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ov7251/common/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ov7251/cpp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ov7251/isp/Makefile \
  media-controller/modules/sensors/chromatix/0309/chromatix_ov7251/postproc/Makefile \
  media-controller/modules/sensors/chromatix/0310/Makefile \
  media-controller/modules/sensors/chromatix/0310/chromatix_ov9281/Makefile \
  media-controller/modules/sensors/chromatix/0310/chromatix_ov9281/3A/Makefile \
  media-controller/modules/sensors/chromatix/0310/chromatix_ov9281/common/Makefile \
  media-controller/modules/sensors/chromatix/0310/chromatix_ov9281/cpp/Makefile \
  media-controller/modules/sensors/chromatix/0310/chromatix_ov9281/isp/Makefile \
  media-controller/modules/sensors/chromatix/0310/chromatix_ov9281/postproc/Makefile \
  media-controller/modules/sensors/actuator/libs/Makefile \
  media-controller/modules/sensors/actuator/libs/lc898212xd_qc2002/Makefile \
  media-controller/modules/sensors/actuator/libs/lc898212xd/Makefile \
  media-controller/modules/sensors/eeprom/libs/Makefile \
  media-controller/modules/sensors/eeprom/libs/eeprom_util/Makefile \
  media-controller/modules/sensors/eeprom/libs/le2464c/Makefile \
  media-controller/modules/sensors/eeprom/libs/onsemi_cat24c32/Makefile \
  media-controller/modules/sensors/eeprom/libs/imx378_gt24c64/Makefile \
  media-controller/modules/sensors/flash/libs/pmic/Makefile \
  media-controller/modules/sensors/flash/libs/bd7710/Makefile \
  media-controller/modules/sensors/flash/libs/Makefile  \
  media-controller/modules/isp2/hw/hdr/hdr46/Makefile \
  media-controller/modules/isp2/hw/hdr/Makefile \
  media-controller/modules/isp2/hw/hdr/hdr48/Makefile \
  media-controller/modules/isp2/hw/clamp/Makefile \
  media-controller/modules/isp2/hw/clamp/clamp40/Makefile \
  media-controller/modules/isp2/hw/rolloff/mesh_rolloff44/Makefile \
  media-controller/modules/isp2/hw/rolloff/Makefile \
  media-controller/modules/isp2/hw/rolloff/mesh_rolloff40/Makefile \
  media-controller/modules/isp2/hw/demux/demux48/Makefile \
  media-controller/modules/isp2/hw/demux/Makefile \
  media-controller/modules/isp2/hw/demux/demux40/Makefile \
  media-controller/modules/isp2/hw/stats/ihist_stats/ihist_stats44/Makefile \
  media-controller/modules/isp2/hw/stats/ihist_stats/Makefile \
  media-controller/modules/isp2/hw/stats/ihist_stats/ihist_stats46/Makefile \
  media-controller/modules/isp2/hw/stats/bhist_stats/bhist_stats44/Makefile \
  media-controller/modules/isp2/hw/stats/bhist_stats/Makefile \
  media-controller/modules/isp2/hw/stats/aec_bg_stats/aec_bg_stats47/Makefile \
  media-controller/modules/isp2/hw/stats/aec_bg_stats/Makefile \
  media-controller/modules/isp2/hw/stats/bf_scale_stats/Makefile \
  media-controller/modules/isp2/hw/stats/bf_scale_stats/bf_scale_stats46/Makefile \
  media-controller/modules/isp2/hw/stats/bf_scale_stats/bf_scale_stats44/Makefile \
  media-controller/modules/isp2/hw/stats/be_stats/Makefile \
  media-controller/modules/isp2/hw/stats/be_stats/be_stats44/Makefile \
  media-controller/modules/isp2/hw/stats/hdr_be_stats/Makefile \
  media-controller/modules/isp2/hw/stats/hdr_be_stats/hdr_be_stats46/Makefile \
  media-controller/modules/isp2/hw/stats/Makefile \
  media-controller/modules/isp2/hw/stats/cs_stats/cs_stats44/Makefile \
  media-controller/modules/isp2/hw/stats/cs_stats/Makefile \
  media-controller/modules/isp2/hw/stats/cs_stats/cs_stats46/Makefile \
  media-controller/modules/isp2/hw/stats/rs_stats/Makefile \
  media-controller/modules/isp2/hw/stats/rs_stats/rs_stats46/Makefile \
  media-controller/modules/isp2/hw/stats/rs_stats/rs_stats44/Makefile \
  media-controller/modules/isp2/hw/stats/bg_stats/bg_stats46/Makefile \
  media-controller/modules/isp2/hw/stats/bg_stats/Makefile \
  media-controller/modules/isp2/hw/stats/bg_stats/bg_stats44/Makefile \
  media-controller/modules/isp2/hw/stats/bf_stats/Makefile \
  media-controller/modules/isp2/hw/stats/bf_stats/bf_stats40/Makefile \
  media-controller/modules/isp2/hw/stats/bf_stats/bf_stats44/Makefile \
  media-controller/modules/isp2/hw/stats/bf_stats/bf_stats47/Makefile \
  media-controller/modules/isp2/hw/wb/wb46/Makefile \
  media-controller/modules/isp2/hw/wb/Makefile \
  media-controller/modules/isp2/hw/wb/wb40/Makefile \
  media-controller/modules/isp2/hw/linearization/Makefile \
  media-controller/modules/isp2/hw/linearization/linearization40/Makefile \
  media-controller/modules/isp2/hw/gtm/gtm46/Makefile \
  media-controller/modules/isp2/hw/gtm/Makefile \
  media-controller/modules/isp2/hw/luma_adaptation/Makefile \
  media-controller/modules/isp2/hw/luma_adaptation/luma_adaptation40/Makefile \
  media-controller/modules/isp2/hw/snr/Makefile \
  media-controller/modules/isp2/hw/snr/snr47/Makefile \
  media-controller/modules/isp2/hw/Makefile \
  media-controller/modules/isp2/hw/template/Makefile \
  media-controller/modules/isp2/hw/abf/abf48/Makefile \
  media-controller/modules/isp2/hw/abf/Makefile \
  media-controller/modules/isp2/hw/abf/abf44/Makefile \
  media-controller/modules/isp2/hw/abf/abf40/Makefile \
  media-controller/modules/isp2/hw/abf/abf47/Makefile \
  media-controller/modules/isp2/hw/sub_module/Makefile \
  media-controller/modules/isp2/hw/gic/gic46/Makefile \
  media-controller/modules/isp2/hw/gic/Makefile \
  media-controller/modules/isp2/hw/gic/gic48/Makefile \
  media-controller/modules/isp2/hw/bpc/bpc48/Makefile \
  media-controller/modules/isp2/hw/bpc/Makefile \
  media-controller/modules/isp2/hw/bpc/bpc40/Makefile \
  media-controller/modules/isp2/hw/bpc/bpc44/Makefile \
  media-controller/modules/isp2/hw/bpc/bpc47/Makefile \
  media-controller/modules/isp2/hw/clf/clf46/Makefile \
  media-controller/modules/isp2/hw/clf/Makefile \
  media-controller/modules/isp2/hw/clf/clf44/Makefile \
  media-controller/modules/isp2/hw/demosaic/demosaic47/Makefile \
  media-controller/modules/isp2/hw/demosaic/demosaic48/Makefile \
  media-controller/modules/isp2/hw/demosaic/Makefile \
  media-controller/modules/isp2/hw/demosaic/demosaic40/Makefile \
  media-controller/modules/isp2/hw/demosaic/demosaic44/Makefile \
  media-controller/modules/isp2/hw/fovcrop/fovcrop40/Makefile \
  media-controller/modules/isp2/hw/fovcrop/Makefile \
  media-controller/modules/isp2/hw/fovcrop/fovcrop46/Makefile \
  media-controller/modules/isp2/hw/cac/Makefile \
  media-controller/modules/isp2/hw/cac/cac47/Makefile \
  media-controller/modules/isp2/hw/pedestal_correct/pedestal_correct46/Makefile \
  media-controller/modules/isp2/hw/pedestal_correct/Makefile \
  media-controller/modules/isp2/hw/ltm/ltm47/Makefile \
  media-controller/modules/isp2/hw/ltm/ltm44/Makefile \
  media-controller/modules/isp2/hw/ltm/Makefile \
  media-controller/modules/isp2/hw/bcc/Makefile \
  media-controller/modules/isp2/hw/bcc/bcc40/Makefile \
  media-controller/modules/isp2/hw/bcc/bcc44/Makefile \
  media-controller/modules/isp2/hw/rccb/Makefile \
  media-controller/modules/isp2/hw/rccb/rccb42/Makefile \
  media-controller/modules/isp2/hw/mce/Makefile \
  media-controller/modules/isp2/hw/mce/mce40/Makefile \
  media-controller/modules/isp2/hw/gamma/gamma44/Makefile \
  media-controller/modules/isp2/hw/gamma/Makefile \
  media-controller/modules/isp2/hw/gamma/gamma40/Makefile \
  media-controller/modules/isp2/hw/abcc/abcc44/Makefile \
  media-controller/modules/isp2/hw/abcc/Makefile \
  media-controller/modules/isp2/hw/color_correct/Makefile \
  media-controller/modules/isp2/hw/color_correct/color_correct46/Makefile \
  media-controller/modules/isp2/hw/color_correct/color_correct40/Makefile \
  media-controller/modules/isp2/hw/chroma_suppress/Makefile \
  media-controller/modules/isp2/hw/chroma_suppress/chroma_suppress40/Makefile \
  media-controller/modules/isp2/hw/pdaf/pdaf48/Makefile \
  media-controller/modules/isp2/hw/pdaf/Makefile \
  media-controller/modules/isp2/hw/black_level/black_level47/Makefile \
  media-controller/modules/isp2/hw/black_level/Makefile \
  media-controller/modules/isp2/hw/black_level/black_level48/Makefile \
  media-controller/modules/isp2/hw/scaler/scaler40/Makefile \
  media-controller/modules/isp2/hw/scaler/scaler44/Makefile \
  media-controller/modules/isp2/hw/scaler/Makefile \
  media-controller/modules/isp2/hw/scaler/scaler46/Makefile \
  media-controller/modules/isp2/hw/sce/sce40/Makefile \
  media-controller/modules/isp2/hw/sce/Makefile \
  media-controller/modules/isp2/hw/color_xform/color_xform46/Makefile \
  media-controller/modules/isp2/hw/color_xform/Makefile \
  media-controller/modules/isp2/hw/color_xform/color_xform40/Makefile \
  media-controller/modules/isp2/hw/chroma_enhan/chroma_enhan40/Makefile \
  media-controller/modules/isp2/hw/chroma_enhan/Makefile
])
