ACLOCAL_AMFLAGS = -I m4

IMX274_CHROMATIX_VIDEO_CPP_PATH = $(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx274/cpp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX274_CHROMATIX_VIDEO_CPP_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX274_CHROMATIX_VIDEO_CPP_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LDFLAGS = -shared
AM_LIBADD = -lcutils

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx274_cpp_svhdr_video)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx274_cpp_svhdr_video.la
libchromatix_imx274_cpp_svhdr_video_la_CPPFLAGS = -I cpp_svhdr_video
libchromatix_imx274_cpp_svhdr_video_la_SOURCES = cpp_svhdr_video/chromatix_imx274_video_svhdr_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx274_cpp_preview)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx274_cpp_preview.la
libchromatix_imx274_cpp_preview_la_CPPFLAGS = -I cpp_preview
libchromatix_imx274_cpp_preview_la_SOURCES = cpp_preview/chromatix_imx274_preview_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx274_cpp_preview_1080p)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx274_cpp_preview_1080p.la
libchromatix_imx274_cpp_preview_1080p_la_CPPFLAGS = -I cpp_preview_1080p
libchromatix_imx274_cpp_preview_1080p_la_SOURCES = cpp_preview_1080p/chromatix_imx274_preview_cpp_1080p.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx274_cpp_preview_ir)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx274_cpp_preview_ir.la
libchromatix_imx274_cpp_preview_ir_la_CPPFLAGS = -I cpp_preview_ir
libchromatix_imx274_cpp_preview_ir_la_SOURCES = cpp_preview_ir/chromatix_imx274_preview_cpp_ir.c

dirs =
SUBDIRS = $(dirs)
