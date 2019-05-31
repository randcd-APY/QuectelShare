OV7251_CHROMATIX_ISP_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_ov7251/isp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(OV7251_CHROMATIX_ISP_PATH)/../../../../module/
AM_CPPFLAGS += -I $(OV7251_CHROMATIX_ISP_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_preview)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_ov7251_preview.la
lihromatix_ov7251_preview_la_CPPFLAGS = -I preview
libchromatix_ov7251_preview_la_SOURCES = preview/chromatix_ov7251_preview.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_hfr_60)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_hfr_60.la
lihromatix_ov7251_hfr_60_la_CPPFLAGS = -I hfr_60
libchromatix_ov7251_hfr_60_la_SOURCES = hfr_60/chromatix_ov7251_hfr_60.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_hfr_90)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_hfr_90.la
lihromatix_ov7251_hfr_90_la_CPPFLAGS = -I hfr_90
libchromatix_ov7251_hfr_90_la_SOURCES = hfr_90/chromatix_ov7251_hfr_90.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_liveshot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_liveshot.la
lihromatix_ov7251_liveshot_la_CPPFLAGS = -I liveshot
libchromatix_ov7251_liveshot_la_SOURCES = liveshot/chromatix_ov7251_liveshot.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov7251_snapshot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_snapshot.la
lihromatix_ov7251_snapshot_la_CPPFLAGS = -I snapshot
libchromatix_ov7251_snapshot_la_SOURCES = snapshot/chromatix_ov7251_snapshot.c

# ---------------------------------------------------------------------------
#                   Make the shared library (libchromatix_ov7251_default_video)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_default_video.la
lihromatix_ov7251_default_video_la_CPPFLAGS = -I video
libchromatix_ov7251_default_video_la_SOURCES = video/chromatix_ov7251_video.c
