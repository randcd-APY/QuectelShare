IMX378_CHROMATIX_ISP_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx378/isp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX378_CHROMATIX_ISP_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX378_CHROMATIX_ISP_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared


# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_hfr_60fps)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx378_hfr_60.la
lihromatix_imx378_hfr_60_la_CPPFLAGS = -I hfr_60
libchromatix_imx378_hfr_60_la_SOURCES = hfr_60/chromatix_imx378_hfr_60.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_preview)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_preview.la
lihromatix_imx378_preview_la_CPPFLAGS = -I preview
libchromatix_imx378_preview_la_SOURCES = preview/chromatix_imx378_preview.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_hfr_120)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_hfr_120.la
lihromatix_imx378_hfr_120_la_CPPFLAGS = -I hfr_120
libchromatix_imx378_hfr_120_la_SOURCES = hfr_120/chromatix_imx378_hfr_120.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_snapshot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_snapshot.la
lihromatix_imx378_snapshot_la_CPPFLAGS = -I snapshot
libchromatix_imx378_snapshot_la_SOURCES = snapshot/chromatix_imx378_snapshot.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_hfr_240)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_hfr_240.la
lihromatix_imx378_hfr_240_la_CPPFLAGS = -I hfr_240
libchromatix_imx378_hfr_240_la_SOURCES = hfr_240/chromatix_imx378_hfr_240.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_video_hdr)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_video_hdr.la
lihromatix_imx378_video_hdr_la_CPPFLAGS = -I video_hdr
libchromatix_imx378_video_hdr_la_SOURCES = video_hdr/chromatix_imx378_video_hdr.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_4K_60fps)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_4K_60.la
lihromatix_imx378_4K_60_la_CPPFLAGS = -I 4K_60
libchromatix_imx378_4K_60_la_SOURCES = 4K_60/chromatix_imx378_4K_60.c

# ---------------------------------------------------------------------------
#                   Make the shared library (libchromatix_imx378_video)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_video.la
lihromatix_imx378_video_la_CPPFLAGS = -I video
libchromatix_imx378_video_la_SOURCES = video/chromatix_imx378_video.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_snapshot_hdr)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_snapshot_hdr.la
lihromatix_imx378_snapshot_hdr_la_CPPFLAGS = -I snapshot_hdr
libchromatix_imx378_snapshot_hdr_la_SOURCES = snapshot_hdr/chromatix_imx378_snapshot_hdr.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_liveshot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_liveshot.la
lihromatix_imx378_liveshot_la_CPPFLAGS = -I liveshot
libchromatix_imx378_liveshot_la_SOURCES = liveshot/chromatix_imx378_liveshot.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_hfr_90)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_hfr_90.la
lihromatix_imx378_hfr_90_la_CPPFLAGS = -I hfr_90
libchromatix_imx378_hfr_90_la_SOURCES = hfr_90/chromatix_imx378_hfr_90.c
