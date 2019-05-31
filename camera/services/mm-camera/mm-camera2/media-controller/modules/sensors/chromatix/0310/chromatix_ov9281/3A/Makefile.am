OV9281_CHROMATIX_3A_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0310/chromatix_ov9281/3A

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS = -I $(OV9281_CHROMATIX_3A_PATH)/../../../../module/
AM_CPPFLAGS = -I $(OV9281_CHROMATIX_3A_PATH)/../../../../../../../../common/
AM_CPPFLAGS = -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared


# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_ov9281_3a_fullsize_preview.la
libchromatix_ov9281_3a_fullsize_preview_la_CPPFLAGS = -I fullsize_preview
libchromatix_ov9281_3a_fullsize_preview_la_SOURCES = fullsize_preview/chromatix_ov9281_fullsize_preview.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov9281_3a_quarter_preview.la
libchromatix_ov9281_3a_quarter_preview_la_CPPFLAGS = -I quarter_preview
libchromatix_ov9281_3a_quarter_preview_la_SOURCES = quarter_preview/chromatix_ov9281_quarter_preview.c