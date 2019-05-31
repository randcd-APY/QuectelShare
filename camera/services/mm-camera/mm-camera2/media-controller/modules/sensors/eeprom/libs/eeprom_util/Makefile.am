# ---------------------------------------------------------------------------
#                       Make the shared library libmmcamera_eeprom_util
# ---------------------------------------------------------------------------
lib_LTLIBRARIES = libmmcamera_eeprom_util.la

libmmcamera_eeprom_util_la_CPPFLAGS = -I ./
libmmcamera_eeprom_util_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_eeprom_util_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
#libmmcamera_eeprom_util_la_CPPFLAGS += -I ../../module/
#libmmcamera_eeprom_util_la_CPPFLAGS += -I ../../../includes

libmmcamera_eeprom_util_la_SOURCES = eeprom_util.c

libmmcamera_eeprom_util_la_LIBADD = -lcutils -llog

libmmcamera_eeprom_util_la_LDFLAGS = -shared

