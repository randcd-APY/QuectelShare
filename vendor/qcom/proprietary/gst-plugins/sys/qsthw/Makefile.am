#Portions of this file are copied from GStreamer template which GStreamer provides under the MIT license.

plugin_LTLIBRARIES = libgstqsthw.la

libgstqsthw_la_SOURCES = \
	gstqsthwplugin.c \
	gstqsthwstrig.c

libgstqsthw_la_CFLAGS = \
        $(QSTHW_CFLAGS) \
	$(GST_PLUGINS_BASE_CFLAGS) \
	$(GST_BASE_CFLAGS) \
	$(GST_CFLAGS) \
	$(ALSA_CFLAGS)
libgstqsthw_la_LIBADD = \
        $(QSTHW_LIBS) \
	-lgstaudio-@GST_API_VERSION@ \
	$(GST_BASE_LIBS) \
	$(ALSA_LIBS)

libgstqsthw_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstqsthw_la_LIBTOOLFLAGS = $(GST_PLUGIN_LIBTOOLFLAGS)

noinst_HEADERS = \
	gstqsthwstrig.h
