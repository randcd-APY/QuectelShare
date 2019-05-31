ACLOCAL_AMFLAGS = -I m4

IMX274_CHROMATIX_3A_PATH = $(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx274/3A

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX274_CHROMATIX_3A_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX274_CHROMATIX_3A_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LDFLAGS = -shared
AM_LIBADD = -lcutils


# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx274_svhdr_video.la
libchromatix_imx274_svhdr_video_la_CPPFLAGS = -I svhdr_video
libchromatix_imx274_svhdr_video_la_SOURCES = svhdr_video/chromatix_imx274_svhdr_video.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx274_zsl_preview.la
libchromatix_imx274_zsl_preview_la_CPPFLAGS = -I zsl_preview
libchromatix_imx274_zsl_preview_la_SOURCES = zsl_preview/chromatix_imx274_zsl_preview.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx274_preview_1080p_3a.la
libchromatix_imx274_preview_1080p_3a_la_CPPFLAGS = -I preview_1080p_3a
libchromatix_imx274_preview_1080p_3a_la_SOURCES = preview_1080p_3a/chromatix_imx274_preview_1080p_3a.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx274_zsl_preview_ir.la
libchromatix_imx274_zsl_preview_ir_la_CPPFLAGS = -I zsl_preview_ir
libchromatix_imx274_zsl_preview_ir_la_SOURCES = zsl_preview_ir/chromatix_imx274_zsl_preview_ir.c

dirs =
SUBDIRS = $(dirs)
