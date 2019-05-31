LOCAL_PATH=$(top_srcdir)/media-controller/modules/sensors/sensor/libs/ov9281

lib_LTLIBRARIES = libmmcamera_ov9281.la

libmmcamera_ov9281_la_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_ov9281_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libmmcamera_ov9281_la_CPPFLAGS += -I $(LOCAL_PATH)
libmmcamera_ov9281_la_SOURCES = ov9281_lib.c
libmmcamera_ov9281_la_LIBADD = -lcutils
libmmcamera_ov9281_la_LDFLAGS = -shared
