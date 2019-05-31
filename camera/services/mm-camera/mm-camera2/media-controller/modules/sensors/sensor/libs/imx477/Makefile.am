LOCAL_PATH=$(top_srcdir)/media-controller/modules/sensors/sensor/libs/imx274

lib_LTLIBRARIES = libmmcamera_imx477.la

libmmcamera_imx477_la_CPPFLAGS  = -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_imx477_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libmmcamera_imx477_la_CPPFLAGS += -I $(LOCAL_PATH)
if ENABLED_IMX477_RAW10_MODES
libmmcamera_imx477_la_CPPFLAGS += -DENABLED_IMX477_RAW10_MODES
endif
libmmcamera_imx477_la_SOURCES = imx477_lib.c
libmmcamera_imx477_la_LIBADD = -lcutils
libmmcamera_imx477_la_LDFLAGS = -shared
