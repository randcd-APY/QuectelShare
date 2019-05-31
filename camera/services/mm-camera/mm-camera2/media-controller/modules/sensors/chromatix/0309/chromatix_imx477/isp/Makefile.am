IMX477_CHROMATIX_ISP_PATH = $(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx477/isp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX477_CHROMATIX_ISP_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX477_CHROMATIX_ISP_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_ defs_i.h
AM_LIBADD = -lcutils
AM_LDFLAGS = -shared

# ---------------------------------------------------------------------------
#             Make the shared library (libchromatix_imx477_hfr_60)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx477_hfr_60.la
libchromatix_imx477_hfr_60_la_CPPFLAGS = -I hfr_60
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_hfr_60_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_hfr_60_la_SOURCES = hfr_60/chromatix_imx477_hfr_60.c

# ---------------------------------------------------------------------------
#            Make the shared library (libchromatix_imx477_preview)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_preview.la
libchromatix_imx477_preview_la_CPPFLAGS = -I preview
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_preview_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_preview_la_SOURCES =  preview/chromatix_imx477_preview.c

# ---------------------------------------------------------------------------
#           Make the shared library (libchromatix_imx477_hfr_120)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_hfr_120.la
libchromatix_imx477_hfr_120_la_CPPFLAGS = -I hfr_120
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_hfr_120_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_hfr_120_la_SOURCES = hfr_120/chromatix_imx477_hfr_120.c

# ---------------------------------------------------------------------------
#             Make the shared library (libchromatix_imx477_snapshot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_snapshot.la
libchromatix_imx477_snapshot_la_CPPFLAGS = -I snapshot
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_snapshot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_snapshot_la_SOURCES = snapshot/chromatix_imx477_snapshot.c

# ---------------------------------------------------------------------------
#           Make the shared library (libchromatix_imx477_hfr_240)
# ---------------------------------------------------------------------------

if ENABLED_IMX477_RAW10_MODES
lib_LTLIBRARIES += libchromatix_imx477_hfr_240.la
libchromatix_imx477_hfr_240_la_CPPFLAGS = -I hfr_240
libchromatix_imx477_hfr_240_la_SOURCES =  hfr_240/chromatix_imx477_hfr_240.c
endif

# ---------------------------------------------------------------------------
#          Make the shared library (libchromatix_imx477_video_hdr)
# ---------------------------------------------------------------------------

if ENABLED_IMX477_RAW10_MODES
else
lib_LTLIBRARIES += libchromatix_imx477_video_hdr.la
libchromatix_imx477_video_hdr_la_CPPFLAGS = -I video_hdr
libchromatix_imx477_video_hdr_la_SOURCES =  video_hdr/chromatix_imx477_video_hdr.c
endif

# ---------------------------------------------------------------------------
#           Make the shared library (libchromatix_imx477_4K_60)
# ---------------------------------------------------------------------------

if ENABLED_IMX477_RAW10_MODES
else
lib_LTLIBRARIES += libchromatix_imx477_4K_60.la
libchromatix_imx477_4K_60_la_CPPFLAGS = -I 4K_60
libchromatix_imx477_4K_60_la_SOURCES =  4K_60/chromatix_imx477_4K_60.c
endif

# ---------------------------------------------------------------------------
#             Make the shared library (libchromatix_imx477_video)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_video.la
libchromatix_imx477_video_la_CPPFLAGS = -I video
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_video_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_video_la_SOURCES =  video/chromatix_imx477_video.c

# ---------------------------------------------------------------------------
#          Make the shared library (libchromatix_imx477_snapshot_hdr)
# ---------------------------------------------------------------------------

if ENABLED_IMX477_RAW10_MODES
else
lib_LTLIBRARIES += libchromatix_imx477_snapshot_hdr.la
libchromatix_imx477_snapshot_hdr_la_CPPFLAGS = -I snapshot_hdr
libchromatix_imx477_snapshot_hdr_la_SOURCES = snapshot_hdr/chromatix_imx477_snapshot_hdr.c
endif

# ---------------------------------------------------------------------------
#           Make the shared library (libchromatix_imx477_liveshot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_liveshot.la
libchromatix_imx477_liveshot_la_CPPFLAGS = -I liveshot
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_liveshot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_liveshot_la_SOURCES =  liveshot/chromatix_imx477_liveshot.c

# ---------------------------------------------------------------------------
#             Make the shared library (libchromatix_imx477_hfr_90)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_hfr_90.la
libchromatix_imx477_hfr_90_la_CPPFLAGS = -I hfr_90
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_hfr_90_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_hfr_90_la_SOURCES = hfr_90/chromatix_imx477_hfr_90.c
