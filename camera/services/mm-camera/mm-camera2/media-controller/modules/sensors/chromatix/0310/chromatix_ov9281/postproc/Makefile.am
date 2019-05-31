OV9281_CHROMATIX_SW_POST_PROC_PATH=$(top_srcdir)/media-controller/modules/sensors/chromatix/0310/chromatix_ov9281/postproc

# ---------------------------------------------------------------------------
#                      Make the shared library (libchromatix_ov9281_sw_post_proc)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libchromatix_ov9281_postproc.la

libchromatix_ov9281_postproc_la_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libchromatix_ov9281_postproc_la_CPPFLAGS += -I $(ov9281_CHROMATIX_SW_POST_PROC_PATH)/../../../../module/
libchromatix_ov9281_postproc_la_CPPFLAGS += -I $(ov9281_CHROMATIX_SW_POST_PROC_PATH)/../../../../../../../../common/
libchromatix_ov9281_postproc_la_CPPFLAGS += -I $(ov9281_CHROMATIX_SW_POST_PROC_PATH)
#libchromatix_ov9281_postproc_la_CPPFLAGS += -include camera_defs_i.h

libchromatix_ov9281_postproc_la_SOURCES = chromatix_ov9281_sw_post_proc.c

libchromatix_ov9281_postproc_la_LIBADD = -lcutils
libchromatix_ov9281_postproc_la_LDFLAGS = -shared
