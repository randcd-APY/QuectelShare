IMX377_CHROMATIX_ISP_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx377/isp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX377_CHROMATIX_ISP_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX377_CHROMATIX_ISP_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_preview)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx377_preview.la
lihromatix_imx377_preview_la_CPPFLAGS = -I preview
libchromatix_imx377_preview_la_SOURCES = preview/chromatix_imx377_preview.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_hfr_60)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_hfr_60.la
lihromatix_imx377_hfr_60_la_CPPFLAGS = -I hfr_60
libchromatix_imx377_hfr_60_la_SOURCES = hfr_60/chromatix_imx377_hfr_60.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_hfr_120)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_hfr_120.la
lihromatix_imx377_hfr_120_la_CPPFLAGS = -I hfr_120
libchromatix_imx377_hfr_120_la_SOURCES = hfr_120/chromatix_imx377_hfr_120.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_snapshot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_snapshot.la
lihromatix_imx377_snapshot_la_CPPFLAGS = -I snapshot
libchromatix_imx377_snapshot_la_SOURCES = snapshot/chromatix_imx377_snapshot.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_hfr_240)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_hfr_240.la
lihromatix_imx377_hfr_240_la_CPPFLAGS = -I hfr_240
libchromatix_imx377_hfr_240_la_SOURCES = hfr_240/chromatix_imx377_hfr_240.c

# ---------------------------------------------------------------------------
#                   Make the shared library (libchromatix_imx377_video)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_video.la
lihromatix_imx377_video_la_CPPFLAGS = -I video
libchromatix_imx377_video_la_SOURCES = video/chromatix_imx377_video.c
