# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx230_postproc)
# ---------------------------------------------------------------------------

MM_CAMERA_PATH = $(top_srcdir)/../

lib_LTLIBRARIES = libchromatix_imx230_postproc.la

libchromatix_imx230_postproc_la_CPPFLAGS = -include camera_defs_i.h
libchromatix_imx230_postproc_la_CPPFLAGS += -I ./
libchromatix_imx230_postproc_la_CPPFLAGS += -I $(MM_CAMERA_PATH)/common/
libchromatix_imx230_postproc_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)

libchromatix_imx230_postproc_la_SOURCES = chromatix_imx230_sw_post_proc.c

libchromatix_imx230_postproc_la_LIBADD = -lcutils
if ENABLE_MM_DEBUG
libchromatix_imx230_postproc_la_LIBADD += -llog
endif
libchromatix_imx230_postproc_la_LDFLAGS = -shared
