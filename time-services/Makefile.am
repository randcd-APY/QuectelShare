AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        $(QMIF_CFLAGS) \
        -I.

h_sources = ./time_service_v01.h \
            ./localdefs.h \
            ./time_genoff_i.h

requiredlibs = $(QMIF_LIBS)

bin_PROGRAMS = time_daemon
time_daemon_SOURCES = time_daemon_mdm.c time_service_v01.c
time_daemon_LDADD = $(requiredlibs)
time_daemon_LDFLAGS = -lpthread  @GLIB_LIBS@
