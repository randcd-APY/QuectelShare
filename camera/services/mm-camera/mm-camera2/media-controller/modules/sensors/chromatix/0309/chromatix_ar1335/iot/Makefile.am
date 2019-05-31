ACLOCAL_AMFLAGS = -I m4

AR1335_CHROMATIX_COMMON_PATH = $(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_ar1335/iot

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(AR1335_CHROMATIX_COMMON_PATH)/../../../../module/
AM_CPPFLAGS += -I $(AR1335_CHROMATIX_COMMON_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LDFLAGS = -shared
AM_LIBADD  = -lcutils

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ar1335_iot_svhdr)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_ar1335_iot_svhdr.la
libchromatix_ar1335_iot_svhdr_la_CPPFLAGS = -I hfr_60
libchromatix_ar1335_iot_svhdr_la_SOURCES = hfr_60/chromatix_ar1335_iot_svhdr.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ar1335_preview_ir)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ar1335_iot_preview_ir.la
libchromatix_ar1335_iot_preview_ir_la_CPPFLAGS = -I preview_ir
libchromatix_ar1335_iot_preview_ir_la_SOURCES = preview_ir/chromatix_ar1335_iot_preview_ir.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ar1335_preview)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ar1335_iot_preview.la
libchromatix_ar1335_iot_preview_la_CPPFLAGS = -I preview
libchromatix_ar1335_iot_preview_la_SOURCES = preview/chromatix_ar1335_iot_preview.c

dirs =
SUBDIRS = $(dirs)
