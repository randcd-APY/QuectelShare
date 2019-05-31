# ---------------------------------------------------------------------------
#                      Make the shared library (libois_bu63164)
# ---------------------------------------------------------------------------
lib_LTLIBRARIES = libois_bu63164.la

libois_bu63164_la_CPPFLAGS = -I ./
libois_bu63164_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/

libois_bu63164_la_SOURCES = bu63164_ois.c

libois_bu63164_la_LIBADD = -lcutils -llog

libois_bu63164_la_LDFLAGS = -shared
