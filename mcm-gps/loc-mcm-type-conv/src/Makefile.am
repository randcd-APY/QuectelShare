AM_CFLAGS = \
            -I. \
            $(QMI_CFLAGS) \
            $(MCMCORE_CFLAGS) \
            $(GPSUTILS_CFLAGS) \
            -DHAVE_LIBLOG \
            -std=c++11

requiredlibs = $(GPSUTILS_LIBS) $(MCMCORE_LIBS)

h_sources = \
    loc_srv_hal_to_mcm_type_conversions.h \
    loc_srv_mcm_to_hal_type_conversions.h

c_sources = \
    loc_srv_hal_to_mcm_type_conversions.cpp \
    loc_srv_mcm_to_hal_type_conversions.cpp


library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)

libloc_mcm_type_conv_la_SOURCES = $(c_sources) $(h_sources)

if USE_GLIB
libloc_mcm_type_conv_la_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
libloc_mcm_type_conv_la_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread @GLIB_LIBS@ -shared -version-info 1:0:0
libloc_mcm_type_conv_la_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
libloc_mcm_type_conv_la_CFLAGS = $(AM_CFLAGS)
libloc_mcm_type_conv_la_LDFLAGS = -Wl,-z,defs -shared -version-info 1:0:0
libloc_mcm_type_conv_la_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

libloc_mcm_type_conv_la_LIBADD = $(requiredlibs)

lib_LTLIBRARIES = libloc_mcm_type_conv.la
