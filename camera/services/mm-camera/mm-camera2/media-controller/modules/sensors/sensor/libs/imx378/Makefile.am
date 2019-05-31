LOCAL_PATH=$(top_srcdir)/media-controller/modules/sensors/sensor/libs/imx274

lib_LTLIBRARIES = libmmcamera_imx378.la

libmmcamera_imx378_la_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_imx378_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libmmcamera_imx378_la_CPPFLAGS += -I $(LOCAL_PATH)
libmmcamera_imx378_la_SOURCES = imx378_lib.c
libmmcamera_imx378_la_LIBADD = -lcutils
libmmcamera_imx378_la_LDFLAGS = -shared
