LOCAL_PATH = $(top_srcdir)/services/tests
bin_PROGRAMS = test_thread_library

if BOARD_QTI_CAMERA_V2
test_thread_library_CFLAGS  = -Werror
endif

test_thread_library_CPPFLAGS  = -DMSM_CAMERA_BIONIC
test_thread_library_CPPFLAGS += -I $(LOCAL_PATH)
test_thread_library_CPPFLAGS += -I $(LOCAL_PATH)/../thread/
test_thread_library_CPPFLAGS += -I $(LOCAL_PATH)/../thread/include

test_thread_library_SOURCES = test_thread_library.c

test_thread_library_LDADD = -lcutils ../thread/libmmcamera_thread_services.la
test_thread_library_LDFLAGS = -shared
