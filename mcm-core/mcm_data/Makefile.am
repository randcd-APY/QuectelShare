AM_CFLAGS = -Wundef \
        -Wno-trigraphs \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        $(QMICLIENTHELPER_CFLAGS) \
        $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS) \
        $(DIAG_CFLAGS)

#refer to Androik.mk for staging header files
AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DAEEINTERFACE_CPLUSPLUS \
        -DFEATURE_SMD \
        -DFEATURE_DIAG_MP_MASTER_APPS \
        -DFEATURE_DATA_LINUX_LE \
        -DFEATURE_MULTIPROCESSOR \
        -DFEATURE_DSUTILS_OFFTARGET \
        -DFEATURE_NO_REX \
        -I $(WORKSPACE)/data/netmgr/inc \
        -I $(WORKSPACE)/data/dsi_netctrl/inc \
        -I $(WORKSPACE)/data/dsi_netctrl/src \
        -I $(WORKSPACE)/data/dsutils/inc \
        -I $(WORKSPACE)/data/data/netmgr/inc \
        -I $(WORKSPACE)/data/data/dsi_netctrl/inc \
        -I $(WORKSPACE)/data/data/dsi_netctrl/src \
        -I $(WORKSPACE)/data/data/dsutils/inc \
        -I $(WORKSPACE)/mcm-core/mcm_ssr_util/ \
        $(DIAG_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(QMIFRAMEWORK_CFLAGS) \
        $(XMLLIB_CFLAGS) \
        -I../ipc/ \
        -I../mcm_ssr_util/

requiredlibs = $(QMIFRAMEWORK_LIBS) $(QMI_LIBS) $(QMICLIENTHELPER_LIBS) $(DIAG_LIBS) -lnetmgr -ldsi_netctrl \
               ../mcm_ssr_util/libmcm_ssr_util.la -lqdi

#############################mcm_data_srv###########################################

mcm_data_srv_sources = mcm_data_srv.c mcm_data_utils.c

bin_PROGRAMS = mcm_data_srv

init_ddir = ${sysconfdir}/init.d
init_d_SCRIPTS = start_mcm_data_srv_le

mcm_data_srv_CC = @CC@
mcm_data_srv_SOURCES = $(mcm_data_srv_sources)
if USE_GLIB
mcm_data_srv_CFLAGS = -Dstrlcpy=strncpy -Dstrlcat=strncat -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
mcm_data_srv_LDFLAGS = -lpthread $(DSUTILS_LIBS) $(XMLLIB_LIBS) $(QMIFRAMEWORK_LIBS) @GLIB_LIBS@ -lqmi_cci  -lqmi -shared -version-info 1:0:0
else
mcm_data_srv_CFLAGS = -Dstrlcpy=strncpy -Dstrlcat=strncat $(AM_CFLAGS) -fPIC
mcm_data_srv_LDFLAGS = -lpthread $(DSUTILS_LIBS) $(XMLLIB_LIBS) $(QMIFRAMEWORK_LIBS) -lqmi_cci  -lqmi -shared -version-info 1:0:0
endif
mcm_data_srv_CPPFLAGS = -Dstrlcpy=strncpy -Dstrlcat=strncat $(AM_CPPFLAGS)
mcm_data_srv_LDADD = $(requiredlibs) ../ipc/libmcmipc.la
