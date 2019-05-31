plugin_LTLIBRARIES = libgstqtivdec.la

# sources used to compile this plug-in
libgstqtivdec_la_SOURCES = gstqtivdec.c \
                           gstqtivdec.h \
                           gstionallocator.h \
                           gstionallocator.c \
                           ion_memory_pool.c \
                           ion_memory_pool.h

# compiler and linker flags used to compile this plugin, set in configure.ac
libgstqtivdec_la_CFLAGS = $(GST_CFLAGS) \
                $(GST_PLUGINS_BASE_CFLAGS) \
                $(GST_BASE_CFLAGS) \
                $(GST_IONBUF_CFLAGS) \
                $(LIBV4L2_CFLAGS)

libgstqtivdec_la_LIBADD = $(GST_LIBS) \
                $(GST_PLUGINS_BASE_LIBS) \
                $(GST_BASE_LIBS) \
                -lgstvideo-$(GST_API_VERSION) \
                -lgstionbuf-@GST_API_VERSION@ \
                -lgstallocators-$(GST_API_VERSION) \
                $(LIBV4L2_LIBS) \
                -lgbm \
                -lgstcrypto

libgstqtivdec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libgstqtivdec_la_LIBTOOLFLAGS = $(GST_PLUGIN_LIBTOOLFLAGS) \
                --tag=disable-static
