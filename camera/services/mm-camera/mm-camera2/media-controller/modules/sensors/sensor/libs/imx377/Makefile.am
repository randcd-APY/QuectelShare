LOCAL_PATH=$(top_srcdir)/media-controller/modules/sensors/sensor/libs/imx377

lib_LTLIBRARIES = libmmcamera_imx377.la

libmmcamera_imx377_la_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_imx377_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libmmcamera_imx377_la_CPPFLAGS += -I $(LOCAL_PATH)
libmmcamera_imx377_la_SOURCES = imx377_lib.c
libmmcamera_imx377_la_LIBADD = -lcutils -llog
libmmcamera_imx377_la_LDFLAGS = -shared
