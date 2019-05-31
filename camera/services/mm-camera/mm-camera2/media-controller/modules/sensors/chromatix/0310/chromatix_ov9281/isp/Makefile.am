OV9281_CHROMATIX_ISP_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0310/chromatix_ov9281/isp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(OV9281_CHROMATIX_ISP_PATH)/../../../../module/
AM_CPPFLAGS += -I $(OV9281_CHROMATIX_ISP_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov9281_preview)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_ov9281_isp_preview.la
lihromatix_ov9281_isp_preview_la_CPPFLAGS = -I preview
libchromatix_ov9281_isp_preview_la_SOURCES = preview/chromatix_ov9281_preview.c
