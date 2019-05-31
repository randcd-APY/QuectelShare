AM_CFLAGS = -Wall -Werror -Wno-unused-value

AM_CXXFLAGS = $(DATA_CFLAGS) \
            $(QMI_CFLAGS) \
            $(QMI_FRAMEWORK_CFLAGS) \
            $(DSUTILS_CFLAGS)
if DEBUG
AM_CFLAGS += -DCONFIG_DEBUG -g
else
AM_CFLAGS += -O2
endif

AM_CFLAGS += -DICNSS_QMI

AM_CFLAGS += -DCONFIG_DEBUG
AM_CFLAGS += -DCONFIG_DEBUG_LOGCAT

AM_CFLAGS += $(QMI_CFLAGS)
AM_CFLAGS += $(QMI_FRAMEWORK_CFLAGS)
AM_CFLAGS += $(LIBNL3_CFLAGS)
AM_CFLAGS += $(LINUX_QUIC_FLAGS)
AM_CFLAGS += $(SYSTEM_CORE_CFLAGS)

AM_CXXFLAGS += $(AM_CFLAGS)

requiredlibs = -lpthread -lcutils -lqmi_cci -lqmi_common_so

c_sources = main.c
c_sources += nl_loop.c
c_sources += debug.c
c_sources += cnss_qmi_client.c
c_sources += wireless_lan_proxy_service_v01.c
c_sources += wlan_firmware_service_v01.c
c_sources += wlfw_qmi_client.c

cnss_daemon_SOURCES = $(c_sources)
cnss_daemon_LDADD = $(requiredlibs)

sbin_PROGRAMS = cnss_daemon
init_ddir = ${sysconfdir}/init.d
dist_init_d_SCRIPTS = start_cnss_daemon
