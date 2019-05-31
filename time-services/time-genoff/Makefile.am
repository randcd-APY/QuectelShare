ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I.

AM_CPPFLAGS = -I$(WORKSPACE)/core/time-services -I$(WORKSPACE)/time-services

library_includedir = $(pkgincludedir)
lib_LTLIBRARIES = libtime_genoff.la
libtime_genoff_la_CC = @CC@
libtime_genoff_la_SOURCES = time_genoff_mdm.c
if USE_GLIB
libtime_genoff_la_CFLAGS = $(AM_CFLAGS) -fPIC -DUSE_GLIB @GLIB_CFLAGS@
libtime_genoff_la_LDFLAGS = -lpthread @GLIB_LIBS@ -shared -version-info $(TIME_GENOFF_LIBRARY_VERSION)
else
libtime_genoff_la_CFLAGS = $(AM_CFLAGS) -fPIC
libtime_genoff_la_LDFLAGS = -shared -version-info $(TIME_GENOFF_LIBRARY_VERSION)
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = time-genoff.pc
EXTRA_DIST = $(pkgconfig_DATA)
