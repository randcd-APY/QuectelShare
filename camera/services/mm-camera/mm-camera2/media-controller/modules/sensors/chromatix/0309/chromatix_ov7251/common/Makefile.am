OV7251_CHROMATIX_COMMON_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_ov7251/common

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(OV7251_CHROMATIX_COMMON_PATH)/../../../../module/
AM_CPPFLAGS += -I $(OV7251_CHROMATIX_COMMON_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared


# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_common)
# ---------------------------------------------------------------------------


lib_LTLIBRARIES = libchromatix_ov7251_common.la
libchromatix_ov7251_common_la_CPPFLAGS = -I $(OV7251_CHROMATIX_COMMON_PATH)
libchromatix_ov7251_common_la_SOURCES = chromatix_ov7251_common.c
