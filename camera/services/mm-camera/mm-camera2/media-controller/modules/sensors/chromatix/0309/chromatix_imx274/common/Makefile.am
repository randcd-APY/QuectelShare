ACLOCAL_AMFLAGS = -I m4

IMX274_CHROMATIX_COMMON_PATH = $(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx274/common

AM_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
AM_CPPFLAGS += -I $(IMX274_CHROMATIX_COMMON_PATH)/../../../../module/
AM_CPPFLAGS += -I $(IMX274_CHROMATIX_COMMON_PATH)/../../../../../../../../common/
AM_CPPFLAGS += -include camera_defs_i.h

AM_LDFLAGS = -shared
AM_LIBADD = -lcutils



# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx274_common)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx274_common.la
libchromatix_imx274_common_la_CPPFLAGS = -I $(IMX274_CHROMATIX_COMMON_PATH)
libchromatix_imx274_common_la_SOURCES = chromatix_imx274_common.c

dirs =
SUBDIRS = $(dirs)
