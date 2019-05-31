LOCAL_PATH=$(top_srcdir)/media-controller/modules/sensors/sensor/libs/imx274

lib_LTLIBRARIES = libmmcamera_imx274.la

libmmcamera_imx274_la_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_imx274_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libmmcamera_imx274_la_CPPFLAGS += -I $(LOCAL_PATH)
libmmcamera_imx274_la_SOURCES = imx274_lib.c
libmmcamera_imx274_la_LIBADD = -lcutils
libmmcamera_imx274_la_LDFLAGS = -shared
