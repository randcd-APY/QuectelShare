ACLOCAL_AMFLAGS = -I m4

AR1335_CHROMATIX_PREVIEW_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_ar1335/isp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(AR1335_CHROMATIX_PREVIEW_PATH)/../../../../module/
AM_CPPFLAGS += -I $(AR1335_CHROMATIX_PREVIEW_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LDFLAGS = -shared
AM_LIBADD = -lcutils

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ar1335_preview)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_ar1335_preview.la
libchromatix_ar1335_preview_la_CPPFLAGS = -I preview
libchromatix_ar1335_preview_la_SOURCES = preview/chromatix_ar1335_preview.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx317_preview)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ar1335_preview_ir.la
libchromatix_ar1335_preview_ir_la_CPPFLAGS = -I preview_ir
libchromatix_ar1335_preview_ir_la_SOURCES = preview_ir/chromatix_ar1335_preview_ir.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ar1335_1080_60)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ar1335_1080_60.la
libchromatix_ar1335_1080_60_la_CPPFLAGS = -I 1080_60
libchromatix_ar1335_1080_60_la_SOURCES = 1080_60/chromatix_ar1335_1080_60.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ar1335_hfr_60)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ar1335_hfr_60.la
libchromatix_ar1335_hfr_60_la_CPPFLAGS = -I hfr_60
libchromatix_ar1335_hfr_60_la_SOURCES = hfr_60/chromatix_ar1335_hfr_60.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ar1335_hfr_50)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ar1335_hfr_50.la
libchromatix_ar1335_hfr_50_CPPFLAGS = -I hfr_50
libchromatix_ar1335_hfr_50_la_SOURCES = hfr_50/chromatix_ar1335_hfr_50.c


dirs =
SUBDIRS = $(dirs)
