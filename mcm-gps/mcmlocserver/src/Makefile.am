AM_CFLAGS = \
            -I. \
            -I$(WORKSPACE)/hardware/libhardware/include/hardware \
            $(QMIF_CFLAGS) \
            $(LOCPLA_CFLAGS) \
            $(LOCCORE_CFLAGS) \
            $(GPSUTILS_CFLAGS) \
            $(LOCSERVICE_CFLAGS) \
            $(MCMCORE_CFLAGS) \
            $(MCMLOCTYPECONV_CFLAGS) \
			$(GLIB_CFLAGS) \
            -std=c++11

requiredlibs = $(QMIF_LIBS) \
               $(LOCCORE_LIBS) \
               $(GPSUTILS_LIBS) \
               $(LOCSERVICE_LIBS) \
               $(LOCATIONAPI_LIBS) \
               $(MCMCORE_LIBS) \
               $(MCMLOCTYPECONV_LIBS) \
			   $(GLIB_LIBS) \
			   -lrt -ldl 

h_sources = \
            loc_srv_utils.h \
            loc_srv.h \
            loc_srv_fwrk_proc_req_hdlrs.h \
            loc_srv_fwrk_requests.h \
            loc_srv_hal_indications.h \
            loc_srv_proc_req_hdlrs.h \
            loc_srv_send_indications.h \
            loc_srv_log.h
c_sources = \
            loc_srv.cpp \
            loc_srv_fwrk_proc_req_hdlrs.cpp \
            loc_srv_fwrk_requests.cpp \
            loc_srv_hal_indications.cpp \
            loc_srv_proc_req_hdlrs.cpp \
            loc_srv_send_indications.cpp

mcmlocserver_SOURCES = $(c_sources) $(h_sources)

if USE_GLIB
mcmlocserver_CFLAGS = -DUSE_GLIB $(AM_CFLAGS)
mcmlocserver_LDFLAGS = -lstdc++ -Wl,-z,defs -lpthread -shared -version-info 1:0:0
mcmlocserver_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS)
else
mcmlocserver_CFLAGS = $(AM_CFLAGS)
mcmlocserver_LDFLAGS = -Wl,-z,defs -shared -version-info 1:0:0
mcmlocserver_CPPFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
endif

init_ddir = $(sysconfdir)/init.d
init_d_SCRIPTS = mcmlocserverd

mcmlocserver_LDADD = $(requiredlibs) $(DRPLUGIN_LIBS) $(DRCLIENT_LIBS) $(GNSSPPS_LIBS)

bin_PROGRAMS = mcmlocserver
