ACLOCAL_AMFLAGS = -I m4

IMX274_CHROMATIX_SW_POST_PROC_PATH = $(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx274/postproc

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX274_CHROMATIX_SW_POST_PROC_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX274_CHROMATIX_SW_POST_PROC_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LDFLAGS = -shared
AM_LIBADD = -lcutils


# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx274_sw_post_proc)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx274_postproc.la
libchromatix_imx274_postproc_la_CPPFLAGS = -I $(IMX274_CHROMATIX_SW_POST_PROC_PATH) 
libchromatix_imx274_postproc_la_SOURCES = chromatix_imx274_sw_post_proc.c

dirs =
SUBDIRS = $(dirs)
