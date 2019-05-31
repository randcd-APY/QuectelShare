plugin_LTLIBRARIES = libgstqomxamrenc.la

libgstqomxamrenc_la_SOURCES = \
	gstomxamrenc.c \
        ../common/gstbufqueue.c


libgstqomxamrenc_la_CFLAGS = \
	-I$(PKG_CONFIG_SYSROOT_DIR)/usr/include/mm-audio/qahw_api/inc \
	-I$(PKG_CONFIG_SYSROOT_DIR)/usr/include/gstreamer-1.0/gst/gst-omx \
	$(QAHW_CFLAGS) \
	$(GST_PLUGINS_BASE_CFLAGS) \
	$(GST_BASE_CFLAGS) \
	$(GST_CFLAGS) \
	$(ALSA_CFLAGS) \
        $(GMODULE_NO_EXPORT_CFLAGS)
libgstqomxamrenc_la_LIBADD = \
        $(GST_LIBS) \
        -lgstaudio-@GST_API_VERSION@ \
	$(QAHW_LIBS) \
	$(GST_BASE_LIBS) \
        -lmm-omxcore \
        -lpthread


libgstqomxamrenc_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstqomxamrenc_la_LIBTOOLFLAGS = $(GST_PLUGIN_LIBTOOLFLAGS)

noinst_HEADERS = \
	gstomxamrenc.h \
        ../common/gstbufqueue.h

