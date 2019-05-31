AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -fpie -pie \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat
AM_CXXFLAGS = \
        $(AM_CFLAGS) \
        -fpermissive

AM_CPPFLAGS = -D__packed__= \
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
        -fpie -pie \
	-I../../qlsyslog/lib \
	-I$(WORKSPACE)/quectel-core/ql-common-api/include \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat


#####################QCMAP_Client##################
requiredlibs =  -lql_common_api \
		-lql_sys_log -lqmi \
		-lqmi_client_qmux -lqmiidl -lqmiservices -lqmi_common_so \
		-lqmi_encdec -lqmi_cci -lqmi_csi -lqmi_sap \
		-lqmi_client_helper -lqmi -lgthread-2.0 \
		 -lglib-2.0 -lpthread -ldsutils -ldiag -ldsi_netctrl -lql_mgmt_client


library_includedir = $(pkgincludedir)

Network_sources = network.c

#QCMAP_CLI_h_sources =

bin_PROGRAMS = Network 
Network_CC = @CC@
Network_SOURCES = $(Network_sources)
#if USE_GLIB
Network_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) #@GLIB_CFLAGS@
Network_LDFLAGS = -lpthread $(XMLLIB_LIBS) #@GLIB_LIBS@
Network_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) #@GLIB_CFLAGS@
#else
#Alpha_CFLAGS = $(AM_CFLAGS) -fPIC
#Alpha_LDFLAGS = -lpthread $(XMLLIB_LIBS)
#Alpha_CPPFLAGS = $(AM_CPPFLAGS)
#endif
Network_LDADD = $(requiredlibs)
