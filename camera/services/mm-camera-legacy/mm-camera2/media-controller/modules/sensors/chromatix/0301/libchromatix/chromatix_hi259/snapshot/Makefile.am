CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/sensors
MM2_PATH = $(top_srcdir)/mm-camera2

lib_LTLIBRARIES = libchromatix_hi259_snapshot.la

libchromatix_hi259_snapshot_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
#libchromatix_hi259_snapshot_la_CFLAGS += -fPIC

libchromatix_hi259_snapshot_la_CFLAGS += -I $(CURR_PATH)/chromatix/0301
libchromatix_hi259_snapshot_la_CFLAGS += -I $(CURR_PATH)/module
libchromatix_hi259_snapshot_la_CFLAGS += -I $(MM2_PATH)/../common
libchromatix_hi259_snapshot_la_CFLAGS += -I $(CURR_PATH)/chromatix/0301/chromatix_hi259/snapshot

libchromatix_hi259_snapshot_la_SOURCES = chromatix_hi259_snapshot.c

libchromatix_hi259_snapshot_la_LDFLAGS = -shared
libchromatix_hi259_snapshot_la_LIBADD = -ldl
