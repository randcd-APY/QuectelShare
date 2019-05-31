AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -g -O0 \
        -fno-inline \
	$(QMIF_CFLAGS) 

h_sources = ./persistent_device_configuration_v01.h

requiredlibs = $(QMIF_LIBS)

bin_PROGRAMS = pdc_daemon
pdc_daemon_SOURCES = persistent_device_configuration_v01.c  pdc_daemon.c
pdc_daemon_LDADD = $(requiredlibs)
pdc_daemon_LDFLAGS = -lpthread $(QMI_LIBS)

