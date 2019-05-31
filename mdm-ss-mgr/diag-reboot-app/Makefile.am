AM_CFLAGS = -Wall \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -I$(WORKSPACE)/common/inc \
        -I$(WORKSPACE)/diag/src \
        -I$(WORKSPACE)/diag/include \
        $(QMI_CFLAGS)

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_SINGLE_LINK \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DFEATURE_LINUX \
        -DFEATURE_NATIVELINUX \
        -DFEATURE_DSM_DUP_ITEMS \
        -DFEATURE_LE_DIAG \
	$(DIAG_CFLAGS)

bin_PROGRAMS = diagrebootapp
diagrebootapp_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
diagrebootapp_SOURCES = registerReboot.c

if USE_GLIB
diagrebootapp_CFLAGS  = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
diagrebootapp_LDFLAGS = $(DIAG_LIBS) $(QMI_LIBS) -lpthread -lrt @GLIB_LIBS@ -shared -version-info 1:0:0
else
diagrebootapp_LDFLAGS = -lpthread -lrt $(DIAG_LIBS) $(QMI_LIBS)
endif
