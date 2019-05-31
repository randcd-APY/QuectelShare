AM_CPPFLAGS = -O2 \
              -D_GNU_SOURCE \
              -Dstrlcpy=strncpy \
              -Dstrlcat=strncat

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

c_sources = btrs_daemon.cpp

requiredlibs = -lm \
               -lpthread \
               -ldl \
               -lqcmap_client

AM_LDFLAGS := $(LDFLAFGS)

bin_PROGRAMS = btrsdaemon
btrsdaemon_CC = @CC@
btrsdaemon_SOURCES = $(c_sources)
btrsdaemon_LDADD = $(requiredlibs)
pkgconfigdir = $(libdir)/pkgconfig
