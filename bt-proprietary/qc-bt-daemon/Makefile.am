AM_CPPFLAGS = -O2 \
              -D_GNU_SOURCE

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS += -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DAEEINTERFACE_CPLUSPLUS \
        -DFEATURE_SMD \
        -DFEATURE_DIAG_MP_MASTER_APPS \
        -DFEATURE_DS_LINUX_ANDROID \
        -DFEATURE_DSS_LINUX \
        -DFEATURE_LINUX \
        -DFEATURE_MULTIPROCESSOR \
        -DFEATURE_DSUTILS_OFFTARGET \
        -DFEATURE_NO_REX \
        -DFEATURE_XMLLIB \
        $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS) \
        $(DIAG_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(XMLLIB_CFLAGS) \
        $(DATA_CFLAGS)

c_sources = qc_bt_main.cpp

requiredlibs = -lm \
               -lpthread \
               -ldl \
               -lqcmap_client \
               -lqmi_cci

if USE_GLIB
 qcbtdaemon_CPPFLAGS  = $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
 qcbtdaemon_LDFLAGS = $(requiredlibs) @GLIB_LIBS@
else
 qcbtdaemon_LDFLAGS = $(requiredlibs)
endif

bin_PROGRAMS = qcbtdaemon
qcbtdaemon_CC = @CC@
qcbtdaemon_SOURCES = $(c_sources)
qcbtdaemon_LDADD = $(requiredlibs)
pkgconfigdir = $(libdir)/pkgconfig
