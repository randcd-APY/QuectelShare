lib_LTLIBRARIES = libgstbthelper.la

libgstbthelper_includedir=$(includedir)/gst
libgstbthelper_include_HEADERS := $(top_srcdir)/bt-helper/gstbthelper.h

libgstbthelper_la_SOURCES = gstbthelper.c


libgstbthelper_la_CFLAGS = \
        -I$(PKG_CONFIG_SYSROOT_DIR)/usr/include/mm-audio/qahw_api/inc \
        $(QAHW_CFLAGS) \
	$(GST_PLUGINS_BASE_CFLAGS) \
	$(GST_BASE_CFLAGS) \
	$(GST_CFLAGS) \
        $(ALSA_CFLAGS)
libgstbthelper_la_LIBADD = \
        $(QAHW_LIBS) \
	-lgstaudio-@GST_API_VERSION@ \
        -lgstapp-1.0 \
	$(GST_BASE_LIBS)



