# ---------------------------------------------------------------------------
#                      Make the shared library (libmmcamera_eebinparse)
# ---------------------------------------------------------------------------

lib_LTLIBRARIES = libmmcamera_eebinparse.la

libmmcamera_eebinparse_la_CPPFLAGS = -I $(MM_CAMERASDK_PATH)/sensor/includes/

libmmcamera_eebinparse_la_SOURCES = eebin_parse.c

libmmcamera_eebinparse_la_LIBADD = -lcutils

if ENABLE_MM_DEBUG
libmmcamera_eebinparse_la_LIBADD += -llog
endif

libmmcamera_eebinparse_la_LDFLAGS = -shared
