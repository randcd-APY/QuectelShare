lib_LTLIBRARIES = libqgstaudiorecorder.la

AM_CPPFLAGS = -O2 \
              -D_GNU_SOURCE

AM_CFLAGS = -Wall \
            -Wundef \
            -Wno-trigraphs \
            -std=c99

libqgstaudiorecorder_includedir=$(includedir)/gst
libqgstaudiorecorder_include_HEADERS := $(top_srcdir)/apps/audiorecorder/gstmediarecorder.h

libqgstaudiorecorder_la_SOURCES = gstmediarecorder.cpp


libqgstaudiorecorder_la_CFLAGS = \
        $(GST_PLUGINS_BASE_CFLAGS) \
        $(GST_BASE_CFLAGS) \
        $(GST_CFLAGS) \
        $(ALSA_CFLAGS) \
        $(AM_CFLAGS)
libqgstaudiorecorder_la_LIBADD = \
        -lgstaudio-@GST_API_VERSION@ \
        -lgstapp-1.0 \
        $(GST_LIBS)

libqgstaudiorecorder_la_CPPFLAGS = $(AM_CPPFLAGS) @GLIB_CFLAGS@
libqgstaudiorecorder_la_CPPFLAGS += $(GST_CFLAGS)
