IMX477_CHROMATIX_CPP_PATH = $(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx477/cpp

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX477_CHROMATIX_CPP_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX477_CHROMATIX_CPP_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared


# ---------------------------------------------------------------------------
#         Make the shared library (libchromatix_imx477_cpp_hfr_120)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx477_cpp_hfr_120.la
libchromatix_imx477_cpp_hfr_120_la_CPPFLAGS = -I hfr_120_cpp
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_cpp_hfr_120_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_cpp_hfr_120_la_SOURCES = cpp_hfr_120/chromatix_imx477_hfr_120_cpp.c

# ---------------------------------------------------------------------------
#          Make the shared library (libchromatix_imx477_cpp_preview)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_cpp_preview.la
libchromatix_imx477_cpp_preview_la_CPPFLAGS  = -I cpp_preview
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_cpp_preview_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_cpp_preview_la_SOURCES =  cpp_preview/chromatix_imx477_preview_cpp.c

# ---------------------------------------------------------------------------
#           Make the shared library (libchromatix_imx477_cpp_4K_60)
# ---------------------------------------------------------------------------

if ENABLED_IMX477_RAW10_MODES
else
lib_LTLIBRARIES += libchromatix_imx477_cpp_4K_60.la
libchromatix_imx477_cpp_4K_60_la_CPPFLAGS =  -I cpp_4K_60
libchromatix_imx477_cpp_4K_60_la_SOURCES =  cpp_4K_60/chromatix_imx477_4K_60_cpp.c
endif

# ---------------------------------------------------------------------------
#         Make the shared library (libchromatix_imx477_cpp_hfr_240)
# ---------------------------------------------------------------------------

if ENABLED_IMX477_RAW10_MODES
lib_LTLIBRARIES += libchromatix_imx477_cpp_hfr_240.la
libchromatix_imx477_cpp_hfr_240_la_CPPFLAGS = -I cpp_hfr_240
libchromatix_imx477_cpp_hfr_240_la_SOURCES = cpp_hfr_240/chromatix_imx477_hfr_240_cpp.c
endif

# ---------------------------------------------------------------------------
#          Make the shared library (libchromatix_imx477_cpp_hfr_60)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_cpp_hfr_60.la
libchromatix_imx477_cpp_hfr_60_la_CPPFLAGS  =  -I cpp_hfr_60
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_cpp_hfr_60_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_cpp_hfr_60_la_SOURCES =  cpp_hfr_60/chromatix_imx477_hfr_60_cpp.c

# ---------------------------------------------------------------------------
#         Make the shared library (libchromatix_imx477_cpp_liveshot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_cpp_liveshot.la
libchromatix_imx477_cpp_liveshot_la_CPPFLAGS  = -I cpp_liveshot
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_cpp_liveshot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_cpp_liveshot_la_SOURCES =  cpp_liveshot/chromatix_imx477_liveshot_cpp.c

# ---------------------------------------------------------------------------
#         Make the shared library (libchromatix_imx477_cpp_video_hdr)
# ---------------------------------------------------------------------------

if ENABLED_IMX477_RAW10_MODES
else
lib_LTLIBRARIES += libchromatix_imx477_cpp_video_hdr.la
libchromatix_imx477_cpp_video_hdr_la_CPPFLAGS = -I cpp_video_hdr
libchromatix_imx477_cpp_video_hdr_la_SOURCES = cpp_video_hdr/chromatix_imx477_video_hdr_cpp.c
endif

# ---------------------------------------------------------------------------
#        Make the shared library (libchromatix_imx477_cpp_snapshot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_cpp_snapshot.la
libchromatix_imx477_cpp_snapshot_la_CPPFLAGS = -I cpp_snapshot
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_cpp_snapshot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_cpp_snapshot_la_SOURCES = cpp_snapshot/chromatix_imx477_snapshot_cpp.c

# ---------------------------------------------------------------------------
#       Make the shared library (libchromatix_imx477_cpp_snapshot_hdr)
# ---------------------------------------------------------------------------

if ENABLED_IMX477_RAW10_MODES
else
lib_LTLIBRARIES += libchromatix_imx477_cpp_snapshot_hdr.la
libchromatix_imx477_cpp_snapshot_hdr_la_CPPFLAGS = -I cpp_snapshot_hdr
libchromatix_imx477_cpp_snapshot_hdr_la_SOURCES = cpp_snapshot_hdr/chromatix_imx477_snapshot_hdr_cpp.c
endif

# ---------------------------------------------------------------------------
#          Make the shared library (libchromatix_imx477_cpp_hfr_90)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_cpp_hfr_90.la
libchromatix_imx477_cpp_hfr_90_la_CPPFLAGS = -I cpp_hfr_90
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_cpp_hfr_90_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_cpp_hfr_90_la_SOURCES = cpp_hfr_90/chromatix_imx477_hfr_90_cpp.c

# ---------------------------------------------------------------------------
#          Make the shared library (libchromatix_imx477_cpp_video)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_cpp_video.la
libchromatix_imx477_cpp_video_la_CPPFLAGS =  -I cpp_video
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_cpp_video_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_cpp_video_la_SOURCES =  cpp_video/chromatix_imx477_video_cpp.c
