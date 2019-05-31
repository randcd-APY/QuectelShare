IMX377_CHROMATIX_3A_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx377/3A

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS = -I $(IMX377_CHROMATIX_3A_PATH)/../../../../module/
AM_CPPFLAGS = -I $(IMX377_CHROMATIX_3A_PATH)/../../../../../../../../common/
AM_CPPFLAGS = -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared


# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx377_3a_default_preview.la
libchromatix_imx377_3a_default_preview_la_CPPFLAGS = -I default_preview
libchromatix_imx377_3a_default_preview_la_SOURCES = default_preview/chromatix_imx377_3a_default_preview.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_3a_default_video.la
libchromatix_imx377_3a_default_video_la_CPPFLAGS = -I default_video
libchromatix_imx377_3a_default_video_la_SOURCES =  default_video/chromatix_imx377_3a_default_video.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_3a_hfr_60.la
libchromatix_imx377_3a_hfr_60_la_CPPFLAGS = -I hfr_60
libchromatix_imx377_3a_hfr_60_la_SOURCES =  hfr_60/chromatix_imx377_3a_hfr_60.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_3a_hfr_120.la
libchromatix_imx377_3a_hfr_120_la_CPPFLAGS = -I hfr_120
libchromatix_imx377_3a_hfr_120_la_SOURCES =  hfr_120/chromatix_imx377_3a_hfr_120.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_3a_hfr_240.la
libchromatix_imx377_3a_hfr_240_la_CPPFLAGS = -I hfr_240
libchromatix_imx377_3a_hfr_240_la_SOURCES =  hfr_240/chromatix_imx377_3a_hfr_240.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_zsl_video.la
libchromatix_imx377_zsl_video_la_CPPFLAGS = -I zsl_video
libchromatix_imx377_zsl_video_la_SOURCES =  zsl_video/chromatix_imx377_zsl_video.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx377_zsl_preview.la
libchromatix_imx377_zsl_preview_la_CPPFLAGS = -I zsl_preview
libchromatix_imx377_zsl_preview_la_SOURCES = zsl_preview/chromatix_imx377_zsl_preview.c
