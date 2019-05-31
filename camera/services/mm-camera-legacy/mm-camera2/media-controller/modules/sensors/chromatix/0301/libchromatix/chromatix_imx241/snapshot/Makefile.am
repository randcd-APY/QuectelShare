
CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/sensors
MM2_PATH = $(top_srcdir)/mm-camera2

lib_LTLIBRARIES = libchromatix_imx241_snapshot.la

libchromatix_imx241_snapshot_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
#libchromatix_imx241_snapshot_la_CFLAGS += -fPIC

libchromatix_imx241_snapshot_la_CFLAGS += -I $(CURR_PATH)/chromatix/0301
libchromatix_imx241_snapshot_la_CFLAGS += -I $(CURR_PATH)/module
libchromatix_imx241_snapshot_la_CFLAGS += -I $(MM2_PATH)/../common
libchromatix_imx241_snapshot_la_CFLAGS += -I $(CURR_PATH)/chromatix/0301/chromatix_imx241/snapshot

libchromatix_imx241_snapshot_la_SOURCES = chromatix_imx241_snapshot.c

libchromatix_imx241_snapshot_la_LDFLAGS = -shared
libchromatix_imx241_snapshot_la_LIBADD = -ldl
