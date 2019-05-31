ACLOCAL_AMFLAGS = -I m4

AR1335_CHROMATIX_SW_POST_PROC_PATH = $(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_ar1335/postproc

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(AR1335_CHROMATIX_SW_POST_PROC_PATH)/../../../../module/
AM_CPPFLAGS += -I $(AR1335_CHROMATIX_SW_POST_PROC_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LDFLAGS = -shared
AM_LIBADD = -lcutils


# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ar1335_sw_post_proc)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_ar1335_postproc.la
libchromatix_ar1335_postproc_la_CPPFLAGS = -I $(AR1335_CHROMATIX_SW_POST_PROC_PATH) 
libchromatix_ar1335_postproc_la_SOURCES = chromatix_ar1335_sw_post_proc.c

dirs =
SUBDIRS = $(dirs)
