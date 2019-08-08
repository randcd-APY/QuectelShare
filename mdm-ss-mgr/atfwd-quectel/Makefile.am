AM_CFLAGS = -Wall -g \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        $(QMIFRAMEWORK_CFLAGS) \
	$(QMI_CFLAGS)

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_SINGLE_LINK \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DFEATURE_LINUX \
        -DFEATURE_NATIVELINUX \
        -DFEATURE_DSM_DUP_ITEMS \
        -DFEATURE_LE_DIAG

init_ddir = ${sysconfdir}/init.d
init_d_SCRIPTS = start_atfwd_daemon_quec

bin_PROGRAMS = atfwd_quectel
atfwd_quectel_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
atfwd_quectel_SOURCES = quectel_at_handle.cpp \
						atfwd_daemon_quectel.c
atfwd_quectel_LDFLAGS = -lpthread $(QMIFRAMEWORK_LIBS) $(QMI_LIBS)
atfwd_quectel_LDADD = $(QMIFRAMEWORK_LIBS) $(QMI_LIBS)
