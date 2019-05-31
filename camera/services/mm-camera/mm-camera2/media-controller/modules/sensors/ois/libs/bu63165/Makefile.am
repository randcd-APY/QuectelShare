lib_LTLIBRARIES = libois_bu63165.la

libois_bu63165_la_CPPFLAGS = -I .
libois_bu63165_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/
libois_bu63165_la_SOURCES = bu63165_ois.c
libois_bu63165_la_LIBADD = -lcutils

if ENABLE_MM_DEBUG
libois_bu63165_la_LIBADD += -llog
endif

libois_bu63165_la_LDFLAGS = -shared
