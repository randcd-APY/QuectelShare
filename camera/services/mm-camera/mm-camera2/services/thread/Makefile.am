LOCAL_PATH = $(top_srcdir)/services/thread

lib_LTLIBRARIES = libmmcamera_thread_services.la

libmmcamera_thread_services_la_CFLAGS = -Werror -O3
if BOARD_QTI_CAMERA_V2
libmmcamera_thread_services_la_CFLAGS += -Wno-error=literal-suffix
endif

libmmcamera_thread_services_la_CPPFLAGS = -I $(LOCAL_PATH)/include

libmmcamera_thread_services_la_SOURCES = services.c session.c memory.c profile.c decision.c core.c dispatcher.c

libmmcamera_thread_services_la_LIBADD = -llog -lutils -lcutils -ldl
libmmcamera_thread_services_la_LDFLAGS = -shared
