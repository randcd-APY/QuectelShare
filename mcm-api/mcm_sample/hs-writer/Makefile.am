AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        $(MCM_CORE_CFLAGS)

AM_CPPFLAGS = $(AM_CFLAGS)

ACLOCAL_AMFLAGS = -I m4

requiredlibs = -ldl \
	       $(MCM_CORE_LIBS)

### Build hs_writer binary
bin_PROGRAMS = hs_writer
hs_writer_CC = @CC@
hs_writer_SOURCES = writer.c


if USE_GLIB
hs_writer_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
hs_writer_CPPFLAGS = -DUSE_GLIB $(AM_CPPFLAGS) @GLIB_CFLAGS@
hs_writer_LDFLAGS = -lpthread @GLIB_LIBS@
else
hs_writer_CFLAGS = $(AM_CFLAGS) -fPIC
hs_writer_CPPFLAGS = $(AM_CPPFLAGS) -fPIC
hs_writer_LDFLAGS = -lpthread
endif
hs_writer_CPPFLAGS = $(AM_CPPFLAGS)
hs_writer_LDADD = $(requiredlibs)
