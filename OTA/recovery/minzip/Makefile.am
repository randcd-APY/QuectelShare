AM_CFLAGS := -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        $(LIBSELINUX_CFLAGS) \
        -D_GNU_SOURCE \
        -D_LARGEFILE64_SOURCE \
        -DUSE_LE_MODE

AM_CPPFLAGS := \
              -DUSE_LE_MODE \
              -I. \
              -I$(WORKSPACE)/external/safe-iop/include \
              -I$(WORKSPACE)/system/core/include

AM_LDFLAGS := -shared -version-info 1:0:0

c_sources := \
        Hash.c \
        SysUtil.c \
        DirUtil.c \
        Inlines.c \
        Zip.c

h_sources := \
        Bits.h \
        DirUtil.h \
        Hash.h \
        Log.h \
        SysUtil.h \
        Zip.h \
        inline_magic.h

required_libs := $(MINCRYPT_LIBS)

library_includedir := $(pkgincludedir)
library_include_HEADERS := $(h_sources)

lib_LTLIBRARIES := libminzip.la
libminzip_la_SOURCES := $(c_sources) $(h_sources)

if USE_GLIB
libminzip_la_CFLAGS := $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libminzip_la_CPPFLAGS := $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libminzip_la_LIBADD := -lm -lpthread @GLIB_LIBS@
libminzip_la_LDFLAGS := -all-static
else
libminzip_la_CFLAGS := $(AM_CFLAGS)
libminzip_la_CPPFLAGS := $(AM_CPPFLAGS)
libminzip_la_LIBADD := -lm -lpthread
libminzip_la_LDFLAGS := -all-static
endif
