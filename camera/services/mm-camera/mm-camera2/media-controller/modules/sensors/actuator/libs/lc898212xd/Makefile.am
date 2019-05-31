# ---------------------------------------------------------------------------
#                       Make the shared library (libactuator_lc898212xd)
# ---------------------------------------------------------------------------
lib_LTLIBRARIES = libactuator_lc898212xd.la

libactuator_lc898212xd_la_CPPFLAGS = -I .
libactuator_lc898212xd_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/

libactuator_lc898212xd_la_SOURCES = lc898212xd_actuator.c

libactuator_lc898212xd_la_LIBADD = -lcutils

if ENABLE_MM_DEBUG
libactuator_lc898212xd_la_LIBADD += -llog
endif

libactuator_lc898212xd_la_LDFLAGS = -shared
