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
              -DUSE_LE_MODE \
              -I. \
              -I.. \
              $(LIBSELINUX_CFLAGS) \
              -I$(WORKSPACE)/system/core/include \
              -I$(WORKSPACE)/system/core/base/include \
              -std=c++11

AM_LDFLAGS := -shared -version-info 1:0:0

c_sources := config.cpp ota_io.cpp

library_includedir := $(pkgincludedir)

requiredlibs := -lz \
               ../minzip/libminzip.la \
               -lbase \
               -lselinux

lib_LTLIBRARIES := libotafault.la

if USE_GLIB
libotafault_la_SOURCES := $(c_sources) $(h_sources)
libotafault_la_CFLAGS := $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libotafault_la_CPPFLAGS := $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libotafault_la_LIBADD := $(requiredlibs) @GLIB_LIBS@
libotafault_la_LDFLAGS := -all-static
else
libotafault_la_SOURCES := $(c_sources) $(h_sources)
libotafault_la_CFLAGS := $(AM_CFLAGS)
libotafault_la_CPPFLAGS := $(AM_CPPFLAGS)
libotafault_la_LIBADD := $(requiredlibs)
libotafault_la_LDFLAGS := -all-static
endif
