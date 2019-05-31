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

### Build home_security binary
bin_PROGRAMS = home_security
home_security_CC = @CC@
home_security_SOURCES = main.c config_file_handling.c data_call_handler.c client.c


if USE_GLIB
home_security_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
home_security_CPPFLAGS = -DUSE_GLIB $(AM_CPPFLAGS) @GLIB_CFLAGS@
home_security_LDFLAGS = -lpthread @GLIB_LIBS@
else
home_security_CFLAGS = $(AM_CFLAGS) -fPIC
home_security_CPPFLAGS = $(AM_CPPFLAGS) -fPIC
home_security_LDFLAGS = -lpthread
endif
home_security_CPPFLAGS = $(AM_CPPFLAGS)
home_security_LDADD = $(requiredlibs)
