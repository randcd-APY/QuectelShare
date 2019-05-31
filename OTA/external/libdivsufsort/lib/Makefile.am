ACLOCAL_AMFLAGS := -I m4

AM_CFLAGS := -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -D_GNU_SOURCE \
        -DHAVE_CONFIG_H=1 \
        -DBUILD_DIVSUFSORT64 \
        -I../android_include \
        -I../include

AM_CPPFLAGS := \
              -I.

AUTOMAKE_OPTIONS = subdir-objects

AM_LDFLAGS := -shared -version-info 1:0:0

c_sources := divsufsort.c \
    sssort.c \
    trsort.c \
    utils.c

library_includedir := $(pkgincludedir)

lib_LTLIBRARIES := libdivsufsort.la
libdivsufsort_la_SOURCES := $(c_sources)

if USE_GLIB
libdivsufsort_la_CFLAGS := $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libdivsufsort_la_CPPFLAGS := $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libdivsufsort_la_LIBADD := -lm -lpthread @GLIB_LIBS@
else
libdivsufsort_la_CFLAGS := $(AM_CFLAGS)
libdivsufsort_la_CPPFLAGS := $(AM_CPPFLAGS)
libdivsufsort_la_LIBADD := -lm -lpthread
endif
