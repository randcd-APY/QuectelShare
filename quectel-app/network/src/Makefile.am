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
        -fpie -pie \
	-I$(WORKSPACE)/quectel-core/ql-common-api/include \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat


#####################QCMAP_Client##################
requiredlibs =  -lql_common_api \
		-lql_sys_log \
		-lqmiservices \
		-lqmi_cci \
		-ldsi_netctrl -lql_mgmt_client

Network_sources = network.c
bin_PROGRAMS = Network 
Network_CC = @CC@
Network_SOURCES = $(Network_sources)
Network_CFLAGS = -DUSE_GLIB $(AM_CFLAGS)
Network_LDFLAGS = -lpthread $(XMLLIB_LIBS)
Network_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS)
Network_LDADD = $(requiredlibs)
