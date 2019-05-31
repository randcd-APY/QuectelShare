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
        -DFEATURE_DATA_PS \
        -DFEATURE_DSS_LINUX \
        -DFEATURE_LINUX \
        -DFEATURE_MULTIPROCESSOR \
        -DFEATURE_DSUTILS_OFFTARGET \
        -DFEATURE_NO_REX \
        -DFEATURE_XMLLIB \
        -DFEATURE_DATA_LOG_QXDM \
        -I../inc \
        $(QMIFRAMEWORK_CFLAGS) \
        $(MCMCORE_CFLAGS) \
        $(QMI_CFLAGS) \
        $(DIAG_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(XMLLIB_CFLAGS) \
        -I../../../ipc/ \
        -I../../../mcm_ssr_util/ \
        $(DATA_CFLAGS)

requiredlibs = -lpthread -lrt \
        ../../../ipc/libmcmipc.la \
        ../../../mcm_ssr_util/libmcm_ssr_util.la


#####################MCM_atcop##################

MCM_atcop_svc_sources =  mcm_atcop_svc_hdlr.c

MCM_atcop_svc_h_sources = mcm_atcop_svc_hdlr.h

bin_PROGRAMS = MCM_atcop_svc

init_ddir = ${sysconfdir}/init.d
init_d_SCRIPTS = start_MCM_atcop_svc_le

MCM_atcop_svc_CC = @CC@
MCM_atcop_svc_SOURCES = $(MCM_atcop_svc_sources) $(MCM_atcop_svc_h_sources)
if USE_GLIB
MCM_atcop_svc_CFLAGS =  -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
MCM_atcop_svc_LDFLAGS = -lpthread $(DSUTILS_LIBS) $(XMLLIB_LIBS) -lpthread -lqmi_csi @GLIB_LIBS@
else
MCM_atcop_svc_CFLAGS =  $(AM_CFLAGS) -fPIC
MCM_atcop_svc_LDFLAGS = -lpthread $(DSUTILS_LIBS) $(XMLLIB_LIBS) -lqmi_csi
endif
MCM_atcop_svc_CPPFLAGS = $(AM_CPPFLAGS)
MCM_atcop_svc_LDADD = $(requiredlibs) $(DIAG_LIBS)
