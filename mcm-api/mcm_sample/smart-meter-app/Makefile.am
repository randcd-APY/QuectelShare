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
               

### Build smart_meter_app binary
bin_PROGRAMS = smart_meter_app smart_meter_change_interval
smart_meter_app_CC = @CC@
smart_meter_app_SOURCES = \
                          config_file_handling.c \
                          smart_meter_app.c


if USE_GLIB
smart_meter_app_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
smart_meter_app_CPPFLAGS = -DUSE_GLIB $(AM_CPPFLAGS) @GLIB_CFLAGS@
smart_meter_app_LDFLAGS = -lpthread @GLIB_LIBS@
else
smart_meter_app_CFLAGS = $(AM_CFLAGS) -fPIC
smart_meter_app_CPPFLAGS = $(AM_CPPFLAGS) -fPIC
smart_meter_app_LDFLAGS = -lpthread
endif
smart_meter_app_CPPFLAGS = $(AM_CPPFLAGS)
smart_meter_app_LDADD = $(requiredlibs)



### Build smart_meter_change_interval binary
smart_meter_change_interval_CC = @CC@
smart_meter_change_interval_SOURCES = smart_meter_change_interval.c

if USE_GLIB
smart_meter_change_interval_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
smart_meter_change_interval_CPPFLAGS = -DUSE_GLIB $(AM_CPPFLAGS) @GLIB_CFLAGS@
smart_meter_change_interval_LDFLAGS = -lpthread @GLIB_LIBS@
else
smart_meter_change_interval_CFLAGS = $(AM_CFLAGS) -fPIC
smart_meter_change_interval_CPPFLAGS = $(AM_CPPFLAGS) -fPIC
smart_meter_change_interval_LDFLAGS = -lpthread
endif
smart_meter_change_interval_CPPFLAGS = $(AM_CPPFLAGS)

