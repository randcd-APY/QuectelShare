# ---------------------------------------------------------------------------
#                      Make the shared library libmmcamera_le2464c_eeprom
# ---------------------------------------------------------------------------
 
lib_LTLIBRARIES = libmmcamera_le2464c_eeprom.la

libmmcamera_le2464c_eeprom_la_CPPFLAGS = -I ./
libmmcamera_le2464c_eeprom_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_le2464c_eeprom_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)

libmmcamera_le2464c_eeprom_la_SOURCES = le2464c_eeprom.c

libmmcamera_le2464c_eeprom_la_LIBADD = -lcutils ../eeprom_util/libmmcamera_eeprom_util.la

if ENABLE_MM_DEBUG
libmmcamera_le2464c_eeprom_la_LIBADD += -llog
endif

libmmcamera_le2464c_eeprom_la_LDFLAGS = -shared
