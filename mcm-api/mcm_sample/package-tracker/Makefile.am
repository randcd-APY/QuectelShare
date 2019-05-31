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

### Build package_tracker binary
bin_PROGRAMS = package_tracker
package_tracker_CC = @CC@
package_tracker_SOURCES = main.c client.c data_call_handler.c gps_handler.c


if USE_GLIB
package_tracker_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
package_tracker_CPPFLAGS = -DUSE_GLIB $(AM_CPPFLAGS) @GLIB_CFLAGS@
package_tracker_LDFLAGS = -lpthread @GLIB_LIBS@
else
package_tracker_CFLAGS = $(AM_CFLAGS) -fPIC
package_tracker_CPPFLAGS = $(AM_CPPFLAGS) -fPIC
package_tracker_LDFLAGS = -lpthread
endif
package_tracker_CPPFLAGS = $(AM_CPPFLAGS)
package_tracker_LDADD = $(requiredlibs)
