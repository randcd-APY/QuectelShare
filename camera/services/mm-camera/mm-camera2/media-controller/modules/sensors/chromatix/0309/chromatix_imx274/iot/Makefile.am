ACLOCAL_AMFLAGS = -I m4

IMX274_CHROMATIX_COMMON_PATH = $(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx274/iot

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX274_CHROMATIX_COMMON_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX274_CHROMATIX_COMMON_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LDFLAGS = -shared
AM_LIBADD  = -lcutils

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx274_common)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx274_iot_preview.la
libchromatix_imx274_iot_preview_la_CPPFLAGS = -I preview
libchromatix_imx274_iot_preview_la_SOURCES = preview/chromatix_imx274_iot_preview.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx274_common)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx274_iot_preview_1080p.la
libchromatix_imx274_iot_preview_1080p_la_CPPFLAGS = -I preview_1080p
libchromatix_imx274_iot_preview_1080p_la_SOURCES = preview_1080p/chromatix_imx274_iot_preview_1080p.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx274_common)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx274_iot_preview_ir.la
libchromatix_imx274_iot_preview_ir_la_CPPFLAGS = -I preview_ir
libchromatix_imx274_iot_preview_ir_la_SOURCES = preview_ir/chromatix_imx274_iot_preview_ir.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx274_common)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx274_iot_svhdr.la
libchromatix_imx274_iot_svhdr_la_CPPFLAGS = -I video_svhdr
libchromatix_imx274_iot_svhdr_la_SOURCES = video_svhdr/chromatix_imx274_iot_svhdr.c

dirs =
SUBDIRS = $(dirs)
