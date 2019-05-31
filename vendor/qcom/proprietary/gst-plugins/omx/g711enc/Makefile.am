plugin_LTLIBRARIES = libgstqomxg711enc.la

libgstqomxg711enc_la_SOURCES = \
	gstomxg711enc.c \
        ../common/gstbufqueue.c


libgstqomxg711enc_la_CFLAGS = \
	-I$(PKG_CONFIG_SYSROOT_DIR)/usr/include/mm-audio/qahw_api/inc \
	-I$(PKG_CONFIG_SYSROOT_DIR)/usr/include/gstreamer-1.0/gst/gst-omx \
	$(QAHW_CFLAGS) \
	$(GST_PLUGINS_BASE_CFLAGS) \
	$(GST_BASE_CFLAGS) \
	$(GST_CFLAGS) \
	$(ALSA_CFLAGS) \
        $(GMODULE_NO_EXPORT_CFLAGS)
libgstqomxg711enc_la_LIBADD = \
        $(GST_LIBS) \
        -lgstaudio-@GST_API_VERSION@ \
	$(QAHW_LIBS) \
	$(GST_BASE_LIBS) \
        -lmm-omxcore \
        -lpthread


libgstqomxg711enc_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstqomxg711enc_la_LIBTOOLFLAGS = $(GST_PLUGIN_LIBTOOLFLAGS)

noinst_HEADERS = \
	gstomxg711enc.h \
        ../common/gstbufqueue.h

