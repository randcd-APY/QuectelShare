if BUILD_DEVICE
AM_CFLAGS := -Wundef \
             -Wstrict-prototypes \
             -Wno-trigraphs \
             -g -O0 \
             -fno-inline \
             -fno-short-enums \
             -fpic \
             -D_GNU_SOURCE \
             -I..

AM_CPPFLAGS := \
              -I. \
              -I.. \
              -I../otafault \
              -I$(WORKSPACE)/system/core/include \
              -I$(WORKSPACE)/system/core/base/include \
              -I$(WORKSPACE)/OTA/system/core/base/include \
              -std=c++11

AM_LDFLAGS := -shared -version-info 1:0:0
c_sources := applypatch.cpp \
             bspatch.cpp \
             bsdiff.cpp \
             imgdiff.cpp \
             imgpatch.cpp \
             freecache.cpp \
             utils.cpp

h_sources := applypatch.h

library_includedir := $(pkgincludedir)
library_include_HEADERS := $(h_sources)
requiredlibs := -lm \
               -lpthread \
               -lz \
               -lbz2 \
               -lmincrypt \
               -lcutils \
               ../mtdutils/libmtdutils.la \
               ../edify/libedify.la \
               ../minzip/libminzip.la \
               ../otafault/libotafault.la

lib_LTLIBRARIES := libapplypatch.la

if USE_GLIB
libapplypatch_la_SOURCES := $(c_sources) $(h_sources)
libapplypatch_la_CFLAGS := $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libapplypatch_la_CPPFLAGS := $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libapplypatch_la_LIBADD := $(requiredlibs) @GLIB_LIBS@
libapplypatch_la_LDFLAGS := -all-static
else
libapplypatch_la_SOURCES := $(c_sources) $(h_sources)
libapplypatch_la_CFLAGS := $(AM_CFLAGS)
libapplypatch_la_CPPFLAGS := $(AM_CPPFLAGS)
libapplypatch_la_LIBADD := $(requiredlibs)
libapplypatch_la_LDFLAGS := -all-static
endif

applypatch_c_sources := applypatch.cpp \
             bspatch.cpp \
             bsdiff.cpp \
             imgpatch.cpp \
             freecache.cpp \
             utils.cpp \
             main.cpp

applypatch_h_sources := applypatch.h utils.h

applypatch_SOURCES := $(applypatch_c_sources) $(applypatch_h_sources)

bin_PROGRAMS := applypatch

if USE_GLIB
applypatch_CFLAGS := $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
applypatch_CPPFLAGS := $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
applypatch_LDADD := $(requiredlibs) -lcrypto @GLIB_LIBS@
else
applypatch_CFLAGS := $(AM_CFLAGS)
applypatch_CPPFLAGS := $(AM_CPPFLAGS)
applypatch_LDADD := $(requiredlibs) -lcrypto
endif

endif

if BUILD_HOST
imgdiff_SOURCES := imgdiff.cpp utils.cpp bsdiff.cpp

if USE_GLIB
imgdiff_CFLAGS := $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
imgdiff_CPPFLAGS := $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
imgdiff_LDADD := -lz -lbz2 @GLIB_LIBS@
imgdiff_LDFLAGS := -all-static
else
imgdiff_CFLAGS := $(AM_CFLAGS)
imgdiff_CPPFLAGS := $(AM_CPPFLAGS)
imgdiff_LDADD := -lz -lbz2
imgdiff_LDFLAGS := -all-static
endif

bin_PROGRAMS := imgdiff
endif
