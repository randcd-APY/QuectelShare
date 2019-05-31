AM_CFLAGS = -Wall -Werror

AM_CXXFLAGS = $(DATA_CFLAGS) \
            $(QMI_CFLAGS) \
            $(QMI_FRAMEWORK_CFLAGS) \
            $(DSUTILS_CFLAGS)
if DEBUG
AM_CFLAGS += -DCONFIG_DEBUG -g
else
AM_CFLAGS += -O2
endif

AM_CFLAGS += -DCONFIG_DEBUG_FILE
AM_CFLAGS += -DCONFIG_DEBUG_SYSLOG

AM_CXXFLAGS += $(AM_CFLAGS)

requiredlibs = -lqcmap_client

c_sources = main.c
c_sources += conn_mgr.cpp
c_sources += nl_loop.c
c_sources += debug.c
c_sources += cnss_dp.c

wlan_services_SOURCES = $(c_sources)
wlan_services_LDADD = $(requiredlibs)

sbin_PROGRAMS = wlan_services
init_ddir = ${sysconfdir}/init.d
dist_init_d_SCRIPTS = start_wlan_services
