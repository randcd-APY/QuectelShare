IMX378_CHROMATIX_CPP_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx378/cpp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX378_CHROMATIX_CPP_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX378_CHROMATIX_CPP_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_hfr_120_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx378_cpp_hfr_120.la
libchromatix_imx378_cpp_hfr_120_la_CPPFLAGS = -I cpp_hfr_120
libchromatix_imx378_cpp_hfr_120_la_SOURCES = cpp_hfr_120/chromatix_imx378_hfr_120_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_preview_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_cpp_preview.la
libchromatix_imx378_cpp_preview_la_CPPFLAGS = -I cpp_preview
libchromatix_imx378_cpp_preview_la_SOURCES = cpp_preview/chromatix_imx378_preview_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_4K_60_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_cpp_4K_60.la
libchromatix_imx378_cpp_4K_60_la_CPPFLAGS = -I cpp_4K_60
libchromatix_imx378_cpp_4K_60_la_SOURCES = cpp_4K_60/chromatix_imx378_4K_60_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_hfr_240_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_cpp_hfr_240.la
libchromatix_imx378_cpp_hfr_240_la_CPPFLAGS = -I cpp_hfr_240
libchromatix_imx378_cpp_hfr_240_la_SOURCES = cpp_hfr_240/chromatix_imx378_hfr_240_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_hfr_60_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_cpp_hfr_60.la
libchromatix_imx378_cpp_hfr_60_la_CPPFLAGS = -I cpp_hfr_60
libchromatix_imx378_cpp_hfr_60_la_SOURCES = cpp_hfr_60/chromatix_imx378_hfr_60_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_liveshot_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_cpp_liveshot.la
libchromatix_imx378_cpp_liveshot_la_CPPFLAGS = -I cpp_liveshot
libchromatix_imx378_cpp_liveshot_la_SOURCES = cpp_liveshot/chromatix_imx378_liveshot_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_video_hdr_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_cpp_video_hdr.la
libchromatix_imx378_cpp_video_hdr_la_CPPFLAGS = -I cpp_video_hdr
libchromatix_imx378_cpp_video_hdr_la_SOURCES = cpp_video_hdr/chromatix_imx378_video_hdr_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_snapshot_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_cpp_snapshot.la
libchromatix_imx378_cpp_snapshot_la_CPPFLAGS = -I cpp_snapshot
libchromatix_imx378_cpp_snapshot_la_SOURCES = cpp_snapshot/chromatix_imx378_snapshot_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_snapshot_hdr_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_cpp_snapshot_hdr.la
libchromatix_imx378_cpp_snapshot_hdr_la_CPPFLAGS = -I cpp_snapshot_hdr
libchromatix_imx378_cpp_snapshot_hdr_la_SOURCES = cpp_snapshot_hdr/chromatix_imx378_snapshot_hdr_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_hfr_90_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_cpp_hfr_90.la
libchromatix_imx378_cpp_hfr_90_la_CPPFLAGS = -I cpp_hfr_90
libchromatix_imx378_cpp_hfr_90_la_SOURCES = cpp_hfr_90/chromatix_imx378_hfr_90_cpp.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_video_cpp)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_cpp_video.la
libchromatix_imx378_cpp_video_la_CPPFLAGS = -I cpp_video
libchromatix_imx378_cpp_video_la_SOURCES = cpp_video/chromatix_imx378_video_cpp.c
