OV7251_CHROMATIX_3A_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_ov7251/3A

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS = -I $(OV7251_CHROMATIX_3A_PATH)/../../../../module/
AM_CPPFLAGS = -I $(OV7251_CHROMATIX_3A_PATH)/../../../../../../../../common/
AM_CPPFLAGS = -include camera_defs_i.h

AM_LIBADD = -lcutils
AM_LDFLAGS = -shared


# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_ov7251_zsl_preview.la
libchromatix_ov7251_zsl_preview_la_CPPFLAGS = -I zsl_preview
libchromatix_ov7251_zsl_preview_la_SOURCES = zsl_preview/chromatix_ov7251_zsl_preview.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_zsl_video.la
libchromatix_ov7251_zsl_video_la_CPPFLAGS = -I zsl_video
libchromatix_ov7251_zsl_video_la_SOURCES =  zsl_video/chromatix_ov7251_zsl_video.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_3a_hfr_60.la
libchromatix_ov7251_3a_hfr_60_la_CPPFLAGS = -I hfr_60
libchromatix_ov7251_3a_hfr_60_la_SOURCES =  hfr_60/chromatix_ov7251_3a_hfr_60.c

# ---------------------------------------------------------------------------
#                      Make the shared library
# ---------------------------------------------------------------------------

lib_LTLIBRARIES += libchromatix_ov7251_3a_hfr_90.la
libchromatix_ov7251_3a_hfr_90_la_CPPFLAGS = -I hfr_90
libchromatix_ov7251_3a_hfr_90_la_SOURCES =  hfr_90/chromatix_ov7251_3a_hfr_90.c
