AM_CFLAGS = -Wall -Werror -g

AM_CPPFLAGS = $(LIBNL_CFLAGS) \
            -I$(WORKSPACE)/external/wpa_supplicant_8/src/drivers \
            -I$(WORKSPACE)/external/wpa_supplicant_8/src/common \
            $(DIAG_CFLAGS) \
            $(DSUTILS_CFLAGS)

AM_CPPFLAGS += $(AM_CFLAGS)

requiredlibs = -lpthread -lwpa_client $(LIBNL_LIBS) $(DIAG_LIBS) $(DSUTILS_LIBS)

scm_src = scm_main.c scm_nl.c scm_wlan.c

if QMI_CLI
AM_CPPFLAGS += $(QMI_FRAMEWORK_CFLAGS) \
               $(QMI_CFLAGS)

AM_CFLAGS += -DSCM_IPC_QMI_CLI

requiredlibs += -lqmi_csi -lqmiservices

scm_src += scm_ipc_qmi.c
endif

scm_SOURCES = $(scm_src)
scm_LDADD = $(requiredlibs)

bin_PROGRAMS = scm

misc_ddir = ${sysconfdir}/misc/wifi
misc_d_SCRIPTS = scm.conf
