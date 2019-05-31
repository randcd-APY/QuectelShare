AM_CFLAGS = -Wall \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
	$(QMI_CFLAGS) \
	$(QMIF_CFLAGS) \
        -lrt

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__= \
	$(QMI_CFLAGS) \
	$(QMIF_CFLAGS)

library_includedir = $(pkgincludedir)

bin_PROGRAMS = fs-scrub-daemon
fs_scrub_daemon_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
fs_scrub_daemon_SOURCES = fs-scrub-daemon.c flash_driver_service_v01.c
fs_scrub_deamon_CFLAGS = $(AM_CFLAGS)
fs_scrub_daemon_CPPFLAGS = $(AM_CPPFLAGS)
fs_scrub_daemon_LDFLAGS = -shared -version-info 1:0:0 -lpthread  $(QMIF_LIBS)
fs_scrub_daemon_LDADD = $(QMIF_LIBS)
if USE_GLIB
 AM_CPPFLAGS  += -DUSE_GLIB @GLIB_CFLAGS@
 fs_scrub_daemon_LDFLAGS += @GLIB_LIBS@
endif