lib_LTLIBRARIES = libgstionbuf-@GST_API_VERSION@.la

libgstionbuf_@GST_API_VERSION@_la_SOURCES = \
	gstionbuf_meta.c

libgstionbuf_@GST_API_VERSION@includedir = \
	$(includedir)/gstreamer-@GST_API_VERSION@/gst/ionbuf

libgstionbuf_@GST_API_VERSION@include_HEADERS = \
	gstionbuf_meta.h

libgstionbuf_@GST_API_VERSION@_la_CFLAGS = \
	$(GST_PLUGINS_BAD_CFLAGS) \
	-DGST_USE_UNSTABLE_API \
	$(GST_CFLAGS)

libgstionbuf_@GST_API_VERSION@_la_LIBADD = \
	$(GST_BASE_LIBS) \
	$(GST_LIBS)

libgstionbuf_@GST_API_VERSION@_la_LDFLAGS = \
	$(GST_LIB_LDFLAGS) \
	$(GST_ALL_LDFLAGS) \
	$(GST_LT_LDFLAGS)
