AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        $(DIAG_CFLAGS) \
        $(DSUTILS_CFLAGS)

AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -I$(WORKSPACE)/qmi/services \
        -I$(WORKSPACE)/qmi-framework/inc \
        -I$(WORKSPACE)/qmi-framework/qcci/inc \
        $(QMI_CFLAGS) \
        $(DIAG_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(XMLLIB_CFLAGS) \
        $(QMIFRAMEWORK_CFLAGS) \
        -I$(topdir)/dsutils/inc

requiredlibs = -lcutils -lpthread -lrt $(DIAG_LIBS)

qmuxbridge_sources = qbridge_main.c \
                     qbridge_smd.c \
                     qbridge_log_qxdm.c \
                     qbridge_dpm.c \
                     qbridge_qmi.c

bin_PROGRAMS = qmuxbridge
qmuxbridge_CC = @CC@
qmuxbridge_SOURCES = $(qmuxbridge_sources)
if USE_GLIB
qmuxbridge_CFLAGS  = -DUSE_GLIB $(AM_CFLAGS) $(DIAG_CFLAGS) @GLIB_CFLAGS@
qmuxbridge_LDFLAGS = $(DSUTILS_LIBS) -lpthread -lrt -lqmi_cci -lqmiservices @GLIB_LIBS@
else
qmuxbridge_CFLAGS  = $(AM_CFLAGS) $(DIAG_CFLAGS) -fPIC
qmuxbridge_LDFLAGS =  $(DSUTILS_LIBS) -lpthread -lrt -lqmi_cci -lqmiservices
endif
qmuxbridge_CPPFLAGS = $(AM_CPPFLAGS)
qmuxbridge_LDADD = $(requiredlibs)
