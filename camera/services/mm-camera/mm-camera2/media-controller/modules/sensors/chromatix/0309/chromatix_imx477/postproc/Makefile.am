IMX477_CHROMATIX_SW_POST_PROC_PATH = $(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx477/postproc

# ---------------------------------------------------------------------------
#           Make the shared library (libchromatix_imx477_postproc)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx477_postproc.la

libchromatix_imx477_postproc_la_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libchromatix_imx477_postproc_la_CPPFLAGS += -I $(IMX477_CHROMATIX_SW_POST_PROC_PATH)/../../../../module/
libchromatix_imx477_postproc_la_CPPFLAGS += -I $(IMX477_CHROMATIX_SW_POST_PROC_PATH)/../../../../../../../../common/
libchromatix_imx477_postproc_la_CPPFLAGS += -include camera_defs_i.h
libchromatix_imx477_postproc_la_CPPFLAGS += -I $(IMX477_CHROMATIX_SW_POST_PROC_PATH)
if ENABLED_IMX477_RAW10_MODES
libchromatix_imx477_postproc_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif

libchromatix_imx477_postproc_la_SOURCES = chromatix_imx477_sw_post_proc.c
libchromatix_imx477_postproc_la_LIBADD = -lcutils
libchromatix_imx477_postproc_la_LDFLAGS = -shared
