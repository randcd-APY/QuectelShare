IMX377_CHROMATIX_SW_POST_PROC_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx377/postproc

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx377_sw_post_proc)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx377_postproc.la

libchromatix_imx377_postproc_la_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libchromatix_imx377_postproc_la_CPPFLAGS += -I $(IMX377_CHROMATIX_SW_POST_PROC_PATH)/../../../../module/
libchromatix_imx377_postproc_la_CPPFLAGS += -I $(IMX377_CHROMATIX_SW_POST_PROC_PATH)/../../../../../../../../common/
libchromatix_imx377_postproc_la_CPPFLAGS += -I $(IMX377_CHROMATIX_SW_POST_PROC_PATH)
libchromatix_imx377_postproc_la_CPPFLAGS += -include camera_defs_i.h

libchromatix_imx377_postproc_la_SOURCES = chromatix_imx377_sw_post_proc.c

libchromatix_imx377_postproc_la_LIBADD = -lcutils
libchromatix_imx377_postproc_la_LDFLAGS = -shared
