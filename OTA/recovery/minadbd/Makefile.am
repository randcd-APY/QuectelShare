AM_CFLAGS := -Wundef \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic

AM_CPPFLAGS := \
              -DUSE_LE_MODE \
              -I. \
              -USE_GLIB \
              -I$(WORKSPACE)/system/core/adb \
              -fpermissive

AM_LDFLAGS := -shared -version-info 1:0:0

cpp_sources := \
        adb_main.cpp \
        fuse_adb_provider.cpp \
        services.cpp

h_sources := \
        fuse_adb_provider.h

library_includedir := $(pkgincludedir)
library_include_HEADERS := $(h_sources)

lib_LTLIBRARIES := libminadbd.la
libminadbd_la_SOURCES := $(cpp_sources) $(h_sources)

requiredlibs := -ladbd -lbase

if USE_GLIB
libminadbd_la_CFLAGS := $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libminadbd_la_CPPFLAGS := $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libminadbd_la_LIBADD := $(requiredlibs) -lm -lpthread @GLIB_LIBS@
#libminadbd_la_LDFLAGS := -all-static
else
libminadbd_la_CFLAGS := $(AM_CFLAGS)
libminadbd_la_CPPFLAGS := $(AM_CPPFLAGS)
libminadbd_la_LIBADD := $(requiredlibs) -lm -lpthread
#libminadbd_la_LDFLAGS := -all-static
endif
