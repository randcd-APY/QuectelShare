# ---------------------------------------------------------------------------
#                      Make the shared library (libflash_bd7710)
# ---------------------------------------------------------------------------
lib_LTLIBRARIES = libflash_bd7710.la

libflash_bd7710_la_CPPFLAGS = -I ./
libflash_bd7710_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/

libflash_bd7710_la_SOURCES = bd7710.c

libflash_bd7710_la_LIBADD = -lcutils

if ENABLE_MM_DEBUG
libflash_bd7710_la_LIBADD += -llog
endif

libflash_bd7710_la_LDFLAGS = -shared
