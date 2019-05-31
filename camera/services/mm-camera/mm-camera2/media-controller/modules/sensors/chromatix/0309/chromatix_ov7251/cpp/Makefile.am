OV7251_CHROMATIX_CPP_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_ov7251/cpp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(OV7251_CHROMATIX_CPP_PATH)/../../../../module/
AM_CPPFLAGS += -I $(OV7251_CHROMATIX_CPP_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared


# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_hfr_60_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_ov7251_hfr_60_cpp.la
libchromatix_ov7251_hfr_60_cpp_la_CPPFLAGS = -I cpp_hfr_60
libchromatix_ov7251_hfr_60_cpp_la_SOURCES = cpp_hfr_60/chromatix_ov7251_hfr_60_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_hfr_90_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_hfr_90_cpp.la
libchromatix_ov7251_hfr_90_cpp_la_CPPFLAGS = -I cpp_hfr_90
libchromatix_ov7251_hfr_90_cpp_la_SOURCES = cpp_hfr_90/chromatix_ov7251_hfr_90_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_preview_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_cpp_preview.la
libchromatix_ov7251_cpp_preview_la_CPPFLAGS = -I cpp_preview
libchromatix_ov7251_cpp_preview_la_SOURCES = cpp_preview/chromatix_ov7251_preview_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_liveshot_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_cpp_liveshot.la
libchromatix_ov7251_cpp_liveshot_la_CPPFLAGS = -I cpp_liveshot
libchromatix_ov7251_cpp_liveshot_la_SOURCES = cpp_liveshot/chromatix_ov7251_liveshot_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_snapshot_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_cpp_snapshot.la
libchromatix_ov7251_cpp_snapshot_la_CPPFLAGS = -I cpp_snapshot
libchromatix_ov7251_cpp_snapshot_la_SOURCES = cpp_snapshot/chromatix_ov7251_snapshot_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_video_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_cpp_video.la
libchromatix_ov7251_cpp_video_la_CPPFLAGS = -I cpp_video
libchromatix_ov7251_cpp_video_la_SOURCES = cpp_video/chromatix_ov7251_video_cpp.c
