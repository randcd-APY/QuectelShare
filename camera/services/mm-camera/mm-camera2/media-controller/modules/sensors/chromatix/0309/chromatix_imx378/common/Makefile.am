IMX378_CHROMATIX_COMMON_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx378/common

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX378_CHROMATIX_COMMON_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX378_CHROMATIX_COMMON_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared


# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_common)
# ---------------------------------------------------------------------------


lib_LTLIBRARIES = libchromatix_imx378_common.la
libchromatix_imx378_common_la_CPPFLAGS = -I $(IMX378_CHROMATIX_COMMON_PATH)
libchromatix_imx378_common_la_SOURCES = chromatix_imx378_common.c
