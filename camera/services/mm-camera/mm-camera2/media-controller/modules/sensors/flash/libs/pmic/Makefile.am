# ---------------------------------------------------------------------------
#                      Make the shared library (libflash_pmic)
# ---------------------------------------------------------------------------
lib_LTLIBRARIES = libflash_pmic.la

libflash_pmic_la_CPPFLAGS = -I ./
libflash_pmic_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/

libflash_pmic_la_SOURCES = pmic_flash.c

libflash_pmic_la_LIBADD = -lcutils
if ENABLE_MM_DEBUG
libflash_pmic_la_LIBADD +=-llog
endif

libflash_pmic_la_LDFLAGS = -shared
