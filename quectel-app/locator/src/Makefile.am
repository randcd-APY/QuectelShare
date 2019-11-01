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

AM_CPPFLAGS = -fpie -pie \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat \
		-I$(WORKSPACE)/quectel-core/ql-common-api/include \
		-I$(WORKSPACE)/qmi/inc \
		-I$(WORKSPACE)/data/dsi_netctrl/inc \
		-I$(WORKSPACE)/qmi-framework/inc \
		-I$(WORKSPACE)/qmi/services


requiredlibs = -lgthread-2.0 \
		 -lglib-2.0 -lpthread  -lql_common_api -lcurl -ldsi_netctrl -lqmi -lql_mcm_api -lql_mgmt_client


library_includedir = $(pkgincludedir)

locator_sources = locator.c

#QCMAP_CLI_h_sources =

bin_PROGRAMS = locator 
locator_CC = @CC@
locator_SOURCES = $(locator_sources)
#if USE_GLIB
locator_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) #@GLIB_CFLAGS@
locator_LDFLAGS = -lpthread $(XMLLIB_LIBS) #@GLIB_LIBS@
locator_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) #@GLIB_CFLAGS@
#else
locator_CFLAGS = $(AM_CFLAGS) -fPIC
locator_LDFLAGS = -lpthread $(XMLLIB_LIBS)
locator_CPPFLAGS = $(AM_CPPFLAGS)
#endif
locator_LDADD = $(requiredlibs)
