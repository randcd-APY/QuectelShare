IMX377_CHROMATIX_IOT_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx377/iot

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX377_CHROMATIX_IOT_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX377_CHROMATIX_IOT_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_default_preview_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx377_default_preview_iot.la
libchromatix_imx377_default_preview_iot_la_CPPFLAGS = -I default_preview
libchromatix_imx377_default_preview_iot_la_SOURCES = default_preview/chromatix_imx377_default_preview_iot.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_default_video_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_default_video_iot.la
libchromatix_imx377_default_video_iot_la_CPPFLAGS = -I default_video
libchromatix_imx377_default_video_iot_la_SOURCES = default_video/chromatix_imx377_default_video_iot.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_hfr_60_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_hfr_60_iot.la
lihchromatix_imx377_hfr_60_iot_la_CPPFLAGS = -I hfr_60
libchromatix_imx377_hfr_60_iot_la_SOURCES = hfr_60/chromatix_imx377_hfr_60_iot.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_hfr_120_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_hfr_120_iot.la
lihromatix_imx377_hfr_120_iot_la_CPPFLAGS = -I hfr_120
libchromatix_imx377_hfr_120_iot_la_SOURCES = hfr_120/chromatix_imx377_hfr_120_iot.c

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_hfr_240_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_hfr_240_iot.la
lihromatix_imx377_hfr_240_iot_la_CPPFLAGS = -I hfr_240
libchromatix_imx377_hfr_240_iot_la_SOURCES = hfr_240/chromatix_imx377_hfr_240_iot.c

# ---------------------------------------------------------------------------
#                   Make the shared library (libchromatix_imx377_zsl_video_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_zsl_video_iot.la
lihchromatix_imx377_zsl_video_iot_la_CPPFLAGS = -I zsl_video
libchromatix_imx377_zsl_video_iot_la_SOURCES = zsl_video/chromatix_imx377_zsl_video_iot.c

# ---------------------------------------------------------------------------
#                   Make the shared library (libchromatix_imx377_zsl_preview_iot)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_zsl_preview_iot.la
lihchromatix_imx377_zsl_preview_iot_la_CPPFLAGS = -I zsl_preview
libchromatix_imx377_zsl_preview_iot_la_SOURCES = zsl_preview/chromatix_imx377_zsl_preview_iot.c
