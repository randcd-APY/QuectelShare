LOCAL_PATH=$(top_srcdir)/media-controller/modules/sensors/sensor/libs/ar1335

lib_LTLIBRARIES = libmmcamera_ar1335.la

libmmcamera_ar1335_la_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_ar1335_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libmmcamera_ar1335_la_CPPFLAGS += -I $(LOCAL_PATH)
libmmcamera_ar1335_la_SOURCES = ar1335_lib.c
libmmcamera_ar1335_la_LIBADD = -lcutils
libmmcamera_ar1335_la_LDFLAGS = -shared
