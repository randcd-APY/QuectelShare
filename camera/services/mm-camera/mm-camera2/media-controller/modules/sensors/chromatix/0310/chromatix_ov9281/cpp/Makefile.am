OV9281_CHROMATIX_CPP_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0310/chromatix_ov9281/cpp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(OV9281_CHROMATIX_CPP_PATH)/../../../../module/
AM_CPPFLAGS += -I $(OV9281_CHROMATIX_CPP_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov9281_preview_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_ov9281_cpp_preview.la
libchromatix_ov9281_cpp_preview_la_CPPFLAGS = -I cpp_preview
libchromatix_ov9281_cpp_preview_la_SOURCES = cpp_preview/chromatix_ov9281_preview_cpp.c

