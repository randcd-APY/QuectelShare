LOCAL_PATH=$(top_srcdir)/media-controller/modules/sensors/sensor/libs/ov7251

lib_LTLIBRARIES = libmmcamera_ov7251.la

libmmcamera_ov7251_la_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_ov7251_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libmmcamera_ov7251_la_CPPFLAGS += -I $(LOCAL_PATH)
libmmcamera_ov7251_la_SOURCES = ov7251_lib.c
libmmcamera_ov7251_la_LIBADD = -lcutils
libmmcamera_ov7251_la_LDFLAGS = -shared
