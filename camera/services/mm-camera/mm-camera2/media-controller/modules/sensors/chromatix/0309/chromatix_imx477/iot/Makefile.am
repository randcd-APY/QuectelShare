IMX477_CHROMATIX_IOT_PATH = $(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx477/iot

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX477_CHROMATIX_IOT_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX477_CHROMATIX_IOT_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared

# ---------------------------------------------------------------------------
#         Make the shared library (libchromatix_imx477_hfr_60_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx477_hfr_60_iot.la
libchromatix_imx477_hfr_60_iot_la_CPPFLAGS = -I hfr_60
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_hfr_60_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_hfr_60_iot_la_SOURCES =  hfr_60/chromatix_imx477_hfr_60_iot.c

# ---------------------------------------------------------------------------
#       Make the shared library (libchromatix_imx477_snapshot_hdr_iot)
# ---------------------------------------------------------------------------

if ENABLED_IMX477_RAW10_MODES
else
lib_LTLIBRARIES += libchromatix_imx477_snapshot_hdr_iot.la
libchromatix_imx477_snapshot_hdr_iot_la_CPPFLAGS = -I hdr_snapshot
libchromatix_imx477_snapshot_hdr_iot_la_SOURCES =  hdr_snapshot/chromatix_imx477_snapshot_hdr_iot.c
endif

# ---------------------------------------------------------------------------
#        Make the shared library (libchromatix_imx477_zsl_preview_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_zsl_preview_iot.la
libchromatix_imx477_zsl_preview_iot_la_CPPFLAGS = -I zsl_preview
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_zsl_preview_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_zsl_preview_iot_la_SOURCES =  zsl_preview/chromatix_imx477_zsl_preview_iot.c

# ---------------------------------------------------------------------------
#      Make the shared library (libchromatix_imx477_quad_s_video_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_quad_s_video_iot.la
libchromatix_imx477_quad_s_video_iot_la_CPPFLAGS = -I quad_s_video
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_quad_s_video_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_quad_s_video_iot_la_SOURCES =  quad_s_video/chromatix_imx477_quad_s_video_iot.c

# ---------------------------------------------------------------------------
#      Make the shared library (libchromatix_imx477_default_preview_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_default_preview_iot.la
libchromatix_imx477_default_preview_iot_la_CPPFLAGS = -I default_preview
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_default_preview_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_default_preview_iot_la_SOURCES =  default_preview/chromatix_imx477_default_preview_iot.c

# ---------------------------------------------------------------------------
#        Make the shared library (libchromatix_imx477_4K_video_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_4K_video_iot.la
libchromatix_imx477_4K_video_iot_la_CPPFLAGS = -I 4K_video
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_4K_video_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_4K_video_iot_la_SOURCES =  4K_video/chromatix_imx477_4K_video_iot.c

# ---------------------------------------------------------------------------
#          Make the shared library (libchromatix_imx477_hfr_120_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_hfr_120_iot.la
libchromatix_imx477_hfr_120_iot_la_CPPFLAGS = -I hfr_120
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_hfr_120_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_hfr_120_iot_la_SOURCES =  hfr_120/chromatix_imx477_hfr_120_iot.c

# ---------------------------------------------------------------------------
#          Make the shared library (libchromatix_imx477_hfr_240_iot)
# ---------------------------------------------------------------------------

if ENABLED_IMX477_RAW10_MODES
lib_LTLIBRARIES += libchromatix_imx477_hfr_240_iot.la
libchromatix_imx477_hfr_240_iot_la_CPPFLAGS = -I hfr_240
libchromatix_imx477_hfr_240_iot_la_SOURCES =  hfr_240/chromatix_imx477_hfr_240_iot.c
endif

# ---------------------------------------------------------------------------
#           Make the shared library (libchromatix_imx477_4K_60_iot)
# ---------------------------------------------------------------------------

if ENABLED_IMX477_RAW10_MODES
else
lib_LTLIBRARIES += libchromatix_imx477_4K_60_iot.la
libchromatix_imx477_4K_60_iot_la_CPPFLAGS = -I 4K_60
libchromatix_imx477_4K_60_iot_la_SOURCES =  4K_60/chromatix_imx477_4K_60_iot.c
endif

# ---------------------------------------------------------------------------
#         Make the shared library (libchromatix_imx477_zsl_video_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_zsl_video_iot.la
libchromatix_imx477_zsl_video_iot_la_CPPFLAGS = -I zsl_video
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_zsl_video_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_zsl_video_iot_la_SOURCES =  zsl_video/chromatix_imx477_zsl_video_iot.c

# ---------------------------------------------------------------------------
#       Make the shared library (libchromatix_imx477_video_hdr_iot)
# ---------------------------------------------------------------------------

if ENABLED_IMX477_RAW10_MODES
else
lib_LTLIBRARIES += libchromatix_imx477_video_hdr_iot.la
libchromatix_imx477_video_hdr_iot_la_CPPFLAGS = -I hdr_video
libchromatix_imx477_video_hdr_iot_la_SOURCES = hdr_video/chromatix_imx477_video_hdr_iot.c
endif

# ---------------------------------------------------------------------------
#      Make the shared library (libchromatix_imx477_default_video_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_default_video_iot.la
libchromatix_imx477_default_video_iot_la_CPPFLAGS = -I default_video
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_default_video_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_default_video_iot_la_SOURCES =  default_video/chromatix_imx477_default_video_iot.c

# ---------------------------------------------------------------------------
#       Make the shared library (libchromatix_imx477_4K_preview_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_4K_preview_iot.la
libchromatix_imx477_4K_preview_iot_la_CPPFLAGS = -I 4K_preview
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_4K_preview_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_4K_preview_iot_la_SOURCES =  4K_preview/chromatix_imx477_4K_preview_iot.c

# ---------------------------------------------------------------------------
#      Make the shared library (libchromatix_imx477_quad_b_preview_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_quad_b_preview_iot.la
libchromatix_imx477_quad_b_preview_iot_la_CPPFLAGS = -I quad_b_preview
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_quad_b_preview_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_quad_b_preview_iot_la_SOURCES =  quad_b_preview/chromatix_imx477_quad_b_preview_iot.c

# ---------------------------------------------------------------------------
#      Make the shared library (libchromatix_imx477_quad_b_video_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_quad_b_video_iot.la
libchromatix_imx477_quad_b_video_iot_la_CPPFLAGS = -I quad_b_video
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_quad_b_video_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_quad_b_video_iot_la_SOURCES =  quad_b_video/chromatix_imx477_quad_b_video_iot.c

# ---------------------------------------------------------------------------
#          Make the shared library (libchromatix_imx477_hfr_90_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_hfr_90_iot.la
libchromatix_imx477_hfr_90_iot_la_CPPFLAGS = -I hfr_90
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_hfr_90_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_hfr_90_iot_la_SOURCES =  hfr_90/chromatix_imx477_hfr_90_iot.c

# ---------------------------------------------------------------------------
#      Make the shared library (libchromatix_imx477_quad_s_preview_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx477_quad_s_preview_iot.la
libchromatix_imx477_quad_s_preview_iot_la_CPPFLAGS = -I quad_s_preview
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_quad_s_preview_iot_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libchromatix_imx477_quad_s_preview_iot_la_SOURCES =  quad_s_preview/chromatix_imx477_quad_s_preview_iot.c
