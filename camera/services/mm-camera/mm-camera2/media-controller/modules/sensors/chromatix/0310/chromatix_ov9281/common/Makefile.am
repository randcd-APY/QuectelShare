OV9281_CHROMATIX_COMMON_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0310/chromatix_ov9281/common

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(OV9281_CHROMATIX_COMMON_PATH)/../../../../module/
AM_CPPFLAGS += -I $(OV9281_CHROMATIX_COMMON_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared


# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov9281_common)
# ---------------------------------------------------------------------------


lib_LTLIBRARIES = libchromatix_ov9281_common.la
libchromatix_ov9281_common_la_CPPFLAGS = -I $(OV9281_CHROMATIX_COMMON_PATH)
libchromatix_ov9281_common_la_SOURCES = chromatix_ov9281_common.c
