
plugin_LTLIBRARIES = libgstcustomtracers.la

libgstcustomtracers_la_SOURCES = \
  gststartuptime.c \
  gsttracers.c

libgstcustomtracers_la_CFLAGS = $(GST_PLUGINS_BASE_CFLAGS) $(GST_BASE_CFLAGS) \
	$(GST_CFLAGS)
libgstcustomtracers_la_LIBADD = $(GST_PLUGINS_BASE_LIBS) $(GST_BASE_LIBS) \
	$(GST_LIBS)
libgstcustomtracers_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstcustomtracers_la_LIBTOOLFLAGS = $(GST_PLUGIN_LIBTOOLFLAGS)

noinst_HEADERS = \
  gststartuptime.h
