ACLOCAL_AMFLAGS = -I m4

AR1335_CHROMATIX_VIDEO_CPP_PATH = $(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_ar1335/cpp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(AR1335_CHROMATIX_VIDEO_CPP_PATH)/../../../../module/
AM_CPPFLAGS += -I $(AR1335_CHROMATIX_VIDEO_CPP_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LDFLAGS = -shared
AM_LIBADD = -lcutils

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ar1335_cpp_hfr_60)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_ar1335_cpp_hfr_60.la
libchromatix_ar1335_cpp_hfr_60_la_CPPFLAGS = -I cpp_hfr_60
libchromatix_ar1335_cpp_hfr_60_la_SOURCES = cpp_hfr_60/chromatix_ar1335_hfr_60_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ar1335_cpp_preview)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ar1335_cpp_preview.la
libchromatix_ar1335_cpp_preview_la_CPPFLAGS = -I cpp_preview
libchromatix_ar1335_cpp_preview_la_SOURCES = cpp_preview/chromatix_ar1335_preview_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ar1335_cpp_preview_ir)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ar1335_cpp_preview_ir.la
libchromatix_ar1335_cpp_preview_ir_la_CPPFLAGS = -I cpp_preview_ir
libchromatix_ar1335_cpp_preview_ir_la_SOURCES = cpp_preview_ir/chromatix_ar1335_preview_cpp_ir.c

dirs =
SUBDIRS = $(dirs)
