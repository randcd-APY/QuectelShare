# ---------------------------------------------------------------------------
#                      Make the shared library libmmcamera_onsemi_cat24c32_eeprom
# ---------------------------------------------------------------------------
 
lib_LTLIBRARIES = libmmcamera_onsemi_cat24c32_eeprom.la

libmmcamera_onsemi_cat24c32_eeprom_la_CPPFLAGS = -I ./
libmmcamera_onsemi_cat24c32_eeprom_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/
libmmcamera_onsemi_cat24c32_eeprom_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)

libmmcamera_onsemi_cat24c32_eeprom_la_SOURCES = onsemi_cat24c32.c

libmmcamera_onsemi_cat24c32_eeprom_la_LIBADD = -lcutils ../eeprom_util/libmmcamera_eeprom_util.la

if ENABLE_MM_DEBUG
libmmcamera_onsemi_cat24c32_eeprom_la_LIBADD += -llog
endif

libmmcamera_onsemi_cat24c32_eeprom_la_LDFLAGS = -shared
