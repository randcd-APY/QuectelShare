# ---------------------------------------------------------------------------
#                      Make the shared library libmmcamera_imx378_gt24c64_eeprom
# ---------------------------------------------------------------------------
 
lib_LTLIBRARIES = libmmcamera_imx378_gt24c64_eeprom.la

libmmcamera_imx378_gt24c64_eeprom_la_CPPFLAGS = -I ./
libmmcamera_imx378_gt24c64_eeprom_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_imx378_gt24c64_eeprom_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)

libmmcamera_imx378_gt24c64_eeprom_la_SOURCES = imx378_gt24c64_eeprom.c

libmmcamera_imx378_gt24c64_eeprom_la_LIBADD = -lcutils ../eeprom_util/libmmcamera_eeprom_util.la

if ENABLE_MM_DEBUG
libmmcamera_imx378_gt24c64_eeprom_la_LIBADD += -llog
endif

libmmcamera_imx378_gt24c64_eeprom_la_LDFLAGS = -shared
