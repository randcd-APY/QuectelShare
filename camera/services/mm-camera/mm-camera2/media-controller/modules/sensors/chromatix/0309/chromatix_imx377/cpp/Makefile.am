IMX377_CHROMATIX_CPP_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx377/cpp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX377_CHROMATIX_CPP_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX377_CHROMATIX_CPP_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_hfr_60_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx377_cpp_hfr_60.la
libchromatix_imx377_cpp_hfr_60_la_CPPFLAGS = -I cpp_hfr_60
libchromatix_imx377_cpp_hfr_60_la_SOURCES = cpp_hfr_60/chromatix_imx377_hfr_60_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_hfr_120_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_cpp_hfr_120.la
libchromatix_imx377_cpp_hfr_120_la_CPPFLAGS = -I cpp_hfr_120
libchromatix_imx377_cpp_hfr_120_la_SOURCES = cpp_hfr_120/chromatix_imx377_hfr_120_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_preview_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_cpp_preview.la
libchromatix_imx377_cpp_preview_la_CPPFLAGS = -I cpp_preview
libchromatix_imx377_cpp_preview_la_SOURCES = cpp_preview/chromatix_imx377_preview_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_hfr_240_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_cpp_hfr_240.la
libchromatix_imx377_cpp_hfr_240_la_CPPFLAGS = -I cpp_hfr_240
libchromatix_imx377_cpp_hfr_240_la_SOURCES = cpp_hfr_240/chromatix_imx377_hfr_240_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_liveshot_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_cpp_liveshot.la
libchromatix_imx377_cpp_liveshot_la_CPPFLAGS = -I cpp_liveshot
libchromatix_imx377_cpp_liveshot_la_SOURCES = cpp_liveshot/chromatix_imx377_liveshot_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_snapshot_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_cpp_snapshot.la
libchromatix_imx377_cpp_snapshot_la_CPPFLAGS = -I cpp_snapshot
libchromatix_imx377_cpp_snapshot_la_SOURCES = cpp_snapshot/chromatix_imx377_snapshot_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_video_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_cpp_video.la
libchromatix_imx377_cpp_video_la_CPPFLAGS = -I cpp_video
libchromatix_imx377_cpp_video_la_SOURCES = cpp_video/chromatix_imx377_video_cpp.c
