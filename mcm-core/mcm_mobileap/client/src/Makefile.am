AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -fno-inline \
        -fno-short-enums \
        -fpic


AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DAEEINTERFACE_CPLUSPLUS \
        -DFEATURE_SMD \
        -DFEATURE_DIAG_MP_MASTER_APPS \
        -DFEATURE_DS_LINUX_ANDROID \
        -DFEATURE_DATA_LINUX_LE \
        -DFEATURE_DSS_LINUX \
        -DFEATURE_LINUX \
        -DFEATURE_MULTIPROCESSOR \
        -DFEATURE_DSUTILS_OFFTARGET \
        -DFEATURE_NO_REX \
        -DFEATURE_XMLLIB \
        $(QMIFRAMEWORK_CFLAGS) \
        -I../inc \
        $(MCMCORE_CFLAGS)

requiredlibs = -lpthread -lrt ../../../client/libmcm.la

if TARGET_IS_9615
AM_CPPFLAGS += -DTARGET_IS_9615
endif
MCM_MOBILEAP_CLI_sources =  mcm_mobileap_cli.h mcm_mobileap_client.c

bin_PROGRAMS = MCM_MOBILEAP_CLI

MCM_MOBILEAP_CLI_CC = @CC@
MCM_MOBILEAP_CLI_SOURCES = $(MCM_MOBILEAP_CLI_sources)
if USE_GLIB
MCM_MOBILEAP_CLI_CFLAGS = -Dstrlcpy=strncpy -Dstrlcat=strncat -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
MCM_MOBILEAP_CLI_LDFLAGS = -lpthread @GLIB_LIBS@
else
MCM_MOBILEAP_CLI_CFLAGS = -Dstrlcpy=strncpy -Dstrlcat=strncat $(AM_CFLAGS) -fPIC
MCM_MOBILEAP_CLI_LDFLAGS = -lpthread
endif
MCM_MOBILEAP_CLI_CPPFLAGS = -Dstrlcpy=strncpy -Dstrlcat=strncat $(AM_CPPFLAGS)
MCM_MOBILEAP_CLI_LDADD = $(requiredlibs)
