AM_CFLAGS := -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic

#AM_LEXFLAGS := -x c

AM_YFLAGS := -v -d

AM_CPPFLAGS := \
              -DUSE_LE_MODE \
              -I. \
              -I$(WORKSPACE)/system/core/base/include \
              -std=c++11 -Wno-unused-parameter -Wno-deprecated-register -g -O0

AM_LDFLAGS := -shared -version-info 1:0:0
c_sources := parser.yy \
             lexer.ll \
             expr.cpp

library_includedir := $(pkgincludedir)

lib_LTLIBRARIES := libedify.la
if USE_GLIB
libedify_la_SOURCES := $(c_sources) $(h_sources)
libedify_la_CFLAGS := $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libedify_la_CPPFLAGS := $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libedify_la_LIBADD := -lm -lpthread @GLIB_LIBS@
libedify_la_LDFLAGS := -all-static
else
libedify_la_SOURCES := $(c_sources) $(h_sources)
libedify_la_CFLAGS := $(AM_CFLAGS)
libedify_la_CPPFLAGS := $(AM_CPPFLAGS)
libedify_la_LIBADD := -lm -lpthread
libedify_la_LDFLAGS := -all-static
endif

if TARGET_SUPPORTS_AB
libedify_la_CPPFLAGS += -DTARGET_SUPPORTS_AB
endif

edify_c_sources := parser.yy \
             lexer.ll \
             expr.cpp \
             main.cpp

edify_SOURCES := $(edify_c_sources)

bin_PROGRAMS := edify

if USE_GLIB
edify_CFLAGS := $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
edify_CPPFLAGS := $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
edify_LDADD := -lm -lpthread -lbase @GLIB_LIBS@
else
edify_CFLAGS := $(AM_CFLAGS)
edify_CPPFLAGS := $(AM_CPPFLAGS)
edify_LDADD := -lm -lpthread -lbase
endif
