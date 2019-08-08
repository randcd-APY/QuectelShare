AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -fno-inline \
        -fno-short-enums \
        -fpic

AM_CXXFLAGS = \
        $(AM_CFLAGS) \
        -fpermissive

if TARGET_IS_9615
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
        -I$(WORKSPACE)/data/dss_new/src/platform/inc \
        -I../inc \
        -I$(WORKSPACE)/data/dsutils/inc \
        -I$(WORKSPACE)/data/netmgr/inc \
        -I$(WORKSPACE)/data/dss_new/inc \
        -I$(WORKSPACE)/data/dss_new/api \
        -I$(WORKSPACE)/data/dss_new/src/api \
        -I$(WORKSPACE)/data/dss_new/src/api/dss \
        -I$(WORKSPACE)/data/dss_new/src/api/netiface \
        -I$(WORKSPACE)/data/dss_new/src/api/utils \
        -I$(WORKSPACE)/data/dss_new/src/dss/inc \
        -I$(WORKSPACE)/data/dss_new/src/dsnet/inc \
        -I$(WORKSPACE)/data/dss_new/src/dssock/inc \
        -I$(WORKSPACE)/data/dss_new/src/dssock/src \
        -I$(WORKSPACE)/data/dss_new/src/dssvc/inc \
        -I$(WORKSPACE)/data/dss_new/src/inet/inc \
        -I$(WORKSPACE)/data/dss_new/src/utils/src \
        -I$(WORKSPACE)/data/dss_new/src/utils/inc \
        -I$(WORKSPACE)/data/dss_new/src/tasks/inc \
        -I$(WORKSPACE)/data/dss_new/src/netiface/src \
        -I$(WORKSPACE)/data/dss_new/src/netiface/inc \
        -I$(WORKSPACE)/data/mobileap/inc \
        -I$(WORKSPACE)/data/mobileap/src \
        -I$(WORKSPACE)/data/dss_new/src/qmiifacectls/src \
        -I$(WORKSPACE)/data/dss_new/src/qmiifacectls/inc \
        -I$(WORKSPACE)/data/dsi_netctrl/inc \
        -I$(WORKSPACE)/data/dsi_netctrl/src \
        $(QMIFRAMEWORK_CFLAGS) \
        $(MCMCORE_CFLAGS) \
        $(QMI_CFLAGS) \
        $(DIAG_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(XMLLIB_CFLAGS) \
        -I$(WORKSPACE)/qmi/services \
        -I../../../ipc/ \
        $(DATA_CFLAGS)
else
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
        -DFEATURE_DATA_LOG_QXDM \
        -I../inc \
        -I$(WORKSPACE)/data/netmgr/inc \
        -I$(WORKSPACE)/data/data/netmgr/inc \
        $(DATA_CFLAGS) \
        $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(MCMCORE_CFLAGS) \
        $(DIAG_CFLAGS) \
        -I../../../ipc/
endif

if TARGET_IS_9615
requiredlibs = -lpthread -lrt \
        $(DATA_LIBS) \
        ../../../ipc/libmcmipc.la
else
requiredlibs = -lpthread -lrt \
       ../../../ipc/libmcmipc.la
endif

#####################MCM_MOBILEAP_ConnectionManager##################
if TARGET_IS_9615
AM_CPPFLAGS += -DTARGET_IS_9615
MCM_MOBILEAP_ConnectionManager_sources = MCM_QCMAP_ConnectionManager.cpp mcm_mobileap_svc_hdlr.cpp
MCM_MOBILEAP_ConnectionManager_h_sources = MCM_QCMAP_ConnectionManager.h mcm_mobileap_svc_hdlr.h
else
MCM_MOBILEAP_ConnectionManager_sources = mcm_mobileap_svc_hdlr.cpp
MCM_MOBILEAP_ConnectionManager_h_sources = mcm_mobileap_svc_hdlr.h
endif
bin_PROGRAMS = MCM_MOBILEAP_ConnectionManager

init_ddir = ${sysconfdir}/init.d
init_d_SCRIPTS = start_MCM_MOBILEAP_ConnectionManager_le

MCM_MOBILEAP_ConnectionManager_CC = @CC@
MCM_MOBILEAP_ConnectionManager_SOURCES = $(MCM_MOBILEAP_ConnectionManager_sources) $(MCM_MOBILEAP_ConnectionManager_h_sources)
if USE_GLIB
MCM_MOBILEAP_ConnectionManager_CFLAGS = -Dstrlcpy=strncpy -Dstrlcat=strncat -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
if TARGET_IS_9615
MCM_MOBILEAP_ConnectionManager_LDFLAGS = $(DSUTILS_LIBS) $(XMLLIB_LIBS) -lqmi_csi -ldsi_netctrl  @GLIB_LIBS@
else
MCM_MOBILEAP_ConnectionManager_LDFLAGS = -lqcmap_client @GLIB_LIBS@
endif
else
MCM_MOBILEAP_ConnectionManager_CFLAGS = -Dstrlcpy=strncpy -Dstrlcat=strncat $(AM_CFLAGS) -fPIC
if TARGET_IS_9615
MCM_MOBILEAP_ConnectionManager_LDFLAGS = $(DSUTILS_LIBS) $(XMLLIB_LIBS) -lqmi_csi -ldsi_netctrl
else
MCM_MOBILEAP_ConnectionManager_LDFLAGS = -lqcmap_client
endif
endif
MCM_MOBILEAP_ConnectionManager_CPPFLAGS = -Dstrlcpy=strncpy -Dstrlcat=strncat $(AM_CPPFLAGS)
MCM_MOBILEAP_ConnectionManager_LDADD = $(requiredlibs) $(DIAG_LIBS)
