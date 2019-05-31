IMX378_CHROMATIX_3A_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx378/3A

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS = -I $(IMX378_CHROMATIX_3A_PATH)/../../../../module/
AM_CPPFLAGS = -I $(IMX378_CHROMATIX_3A_PATH)/../../../../../../../../common/
AM_CPPFLAGS = -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx378_4K_60_3a.la
libchromatix_imx378_4K_60_3a_la_CPPFLAGS = -I 4K_60
libchromatix_imx378_4K_60_3a_la_SOURCES =  4K_60/chromatix_imx378_4K_60_3a.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_4K_preview.la
libchromatix_imx378_4K_preview_la_CPPFLAGS = -I 4K_preview
libchromatix_imx378_4K_preview_la_SOURCES =  4K_preview/chromatix_imx378_4K_preview.c
# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_4K_video.la
libchromatix_imx378_4K_video_la_CPPFLAGS = -I 4K_video
libchromatix_imx378_4K_video_la_SOURCES = 4K_video/chromatix_imx378_4K_video.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_default_preview.la
libchromatix_imx378_default_preview_la_CPPFLAGS = -I default_preview
libchromatix_imx378_default_preview_la_SOURCES = default_preview/chromatix_imx378_default_preview.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_default_video.la
libchromatix_imx378_default_video_la_CPPFLAGS = -I default_video
libchromatix_imx378_default_video_la_SOURCES =  default_video/chromatix_imx378_default_video.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_hdr_snapshot_3a.la
libchromatix_imx378_hdr_snapshot_3a_la_CPPFLAGS = -I hdr_snapshot
libchromatix_imx378_hdr_snapshot_3a_la_SOURCES = hdr_snapshot/chromatix_imx378_snapshot_hdr_3a.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_hdr_video_3a.la
libchromatix_imx378_hdr_video_3a_la_CPPFLAGS = -I hdr_video
libchromatix_imx378_hdr_video_3a_la_SOURCES =  hdr_video/chromatix_imx378_video_hdr_3a.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_hfr_60_3a.la
libchromatix_imx378_hfr_60_3a_la_CPPFLAGS = -I hfr_60 
libchromatix_imx378_hfr_60_3a_la_SOURCES =  hfr_60/chromatix_imx378_hfr_60_3a.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_hfr_90_3a.la
libchromatix_imx378_hfr_90_3a_la_CPPFLAGS = -I hfr_90
libchromatix_imx378_hfr_90_3a_la_SOURCES =  hfr_90/chromatix_imx378_hfr_90_3a.c
# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_hfr_120_3a.la
libchromatix_imx378_hfr_120_3a_la_CPPFLAGS = -I hfr_120
libchromatix_imx378_hfr_120_3a_la_SOURCES =  hfr_120/chromatix_imx378_hfr_120_3a.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_hfr_240_3a.la
libchromatix_imx378_hfr_240_3a_la_CPPFLAGS = -I hfr_240
libchromatix_imx378_hfr_240_3a_la_SOURCES =  hfr_240/chromatix_imx378_hfr_240_3a.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_zsl_video.la
libchromatix_imx378_zsl_video_la_CPPFLAGS = -I zsl_video
libchromatix_imx378_zsl_video_la_SOURCES =  zsl_video/chromatix_imx378_zsl_video.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_imx378_zsl_preview.la
libchromatix_imx378_zsl_preview_la_CPPFLAGS = -I zsl_preview
libchromatix_imx378_zsl_preview_la_SOURCES = zsl_preview/chromatix_imx378_zsl_preview.c
