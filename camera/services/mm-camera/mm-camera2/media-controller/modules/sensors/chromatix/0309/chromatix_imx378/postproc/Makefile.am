IMX378_CHROMATIX_SW_POST_PROC_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0309/chromatix_imx378/postproc

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_imx378_sw_post_proc)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_imx378_postproc.la

libchromatix_imx378_postproc_la_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libchromatix_imx378_postproc_la_CPPFLAGS += -I $(IMX378_CHROMATIX_SW_POST_PROC_PATH)/../../../../module/
libchromatix_imx378_postproc_la_CPPFLAGS += -I $(IMX378_CHROMATIX_SW_POST_PROC_PATH)/../../../../../../../../common/
libchromatix_imx378_postproc_la_CPPFLAGS += -I $(IMX378_CHROMATIX_SW_POST_PROC_PATH)
libchromatix_imx378_postproc_la_CPPFLAGS += -include camera_defs_i.h

libchromatix_imx378_postproc_la_SOURCES = chromatix_imx378_sw_post_proc.c

libchromatix_imx378_postproc_la_LIBADD = -lcutils
libchromatix_imx378_postproc_la_LDFLAGS = -shared
