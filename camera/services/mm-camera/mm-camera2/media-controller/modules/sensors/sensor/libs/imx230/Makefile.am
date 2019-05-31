# ---------------------------------------------------------------------------
#                        Make the shared library (libmmcamera_imx230)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libmmcamera_imx230.la

libmmcamera_imx230_la_CFLAGS = -Werror

libmmcamera_imx230_la_CPPFLAGS = -I.
libmmcamera_imx230_la_CPPFLAGS += -I$(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_imx230_la_CPPFLAGS += -I$(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)

libmmcamera_imx230_la_SOURCES = imx230_lib.c

libmmcamera_imx230_la_LIBADD = -lcutils -llog

libmmcamera_imx230_la_LDFLAGS = -shared
