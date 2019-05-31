AM_CFLAGS = -Wall \
    -Wundef \
    -Wstrict-prototypes \
    -Wno-trigraphs \
    -std=c99

c_sources = \
    bt_nv.cpp

requiredlibs = -lpthread
library_includedir = $(pkgincludedir)\
library_include_HEADERS = $(h_sources)
lib_LTLIBRARIES = libbtnv.la
libbtnv_la_CC = @CC@
libbtnv_la_SOURCES = $(c_sources)
libbtnv_la_CFLAGS = $(AM_CFLAGS) -fPIC
libbtnv_la_LDFLAGS = -shared

bin_PROGRAMS := hci_qcomm_init

hci_qcomm_init_CFLAGS := -Wall
hci_qcomm_init_CFLAGS += -Wundef
hci_qcomm_init_CFLAGS += -Wstrict-prototypes
hci_qcomm_init_CFLAGS += -Wno-trigraphs
hci_qcomm_init_CFLAGS += -g

hci_qcomm_init_CPPFLAGS := -D__packed__=
hci_qcomm_init_CPPFLAGS += -DIMAGE_APPS_PROC
hci_qcomm_init_CPPFLAGS += -DFEATURE_Q_SINGLE_LINK
hci_qcomm_init_CPPFLAGS += -DFEATURE_Q_NO_SELF_QPTR
hci_qcomm_init_CPPFLAGS += -DFEATURE_LINUX
hci_qcomm_init_CPPFLAGS += -DFEATURE_NATIVELINUX
hci_qcomm_init_CPPFLAGS += -DFEATURE_DSM_DUP_ITEMS
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_SLEEP
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_BTS4020_BDB0
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_BTS4020_BDB1
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_BTS4020_R3
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_BTS4021_B1
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_BTS4025_B0
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_BTS4025_B1
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_BTS4025_B2
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_BTS4025_B3
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_MARIMBA_A0
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_MARIMBA_B0
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_MARIMBA_B1
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_BAHAMA_A0
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_BAHAMA_B0
# Enable SW in band sleep when qualcomm-ibs device type is part
# of hciattach.c
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_CLASS2
hci_qcomm_init_CPPFLAGS += -DPLATFORM_LINUX_PC
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_SYSTEM_CLOCK_XO_SUPPORT
hci_qcomm_init_CPPFLAGS += -DFEATURE_BT_QSOC_SW_INBAND_SLEEP
hci_qcomm_init_CPPFLAGS += -DBT_NVM_MBA_B0_LOWER_TX_POWER
hci_qcomm_init_CPPFLAGS += -DBT_QSOC_ENABLE_CLOCK_SHARING
hci_qcomm_init_CPPFLAGS += -DBT_QSOC_HCI_BAUD_RATE=3000000
hci_qcomm_init_CPPFLAGS += -DBT_QSOC_REF_CLOCK=19200000
hci_qcomm_init_CPPFLAGS += -DBT_QSOC_GET_ITEMS_FROM_PERSIST

if MDM_ROME
hci_qcomm_init_CFLAGS += -DBT_SOC_TYPE_ROME
hci_qcomm_init_CPPFLAGS += -DBT_SOC_TYPE_ROME
else
if MDM_PRONTO
hci_qcomm_init_CFLAGS += -DHCI_USE_MCT
hci_qcomm_init_CPPFLAGS += -DHCI_USE_MCT
endif
endif

if MDM_CHEROKEE
hci_qcomm_init_CFLAGS += -DBT_SOC_TYPE_CHEROKEE
hci_qcomm_init_CPPFLAGS += -DBT_SOC_TYPE_CHEROKEE
endif

hci_qcomm_init_SOURCES := bthci_qcomm_linux.cpp
hci_qcomm_init_SOURCES += bthci_qcomm_linux_uart.c
hci_qcomm_init_SOURCES += bthci_qcomm_common.c
hci_qcomm_init_SOURCES += btqsocnvm.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4020_BDB0_19P2Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4020_BDB0_32Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4020_BDB1_19P2Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4020_BDB1_32Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4020_R3_19P2Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4020_R3_32Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4021_B1_19P2Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4021_B1_32Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4025_B0_19P2Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4025_B0_32Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4025_B1_19P2Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4025_B1_32Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4025_B2_19P2Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4025_B2_32Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_MARIMBA_A0.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_MARIMBA_B0.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_MARIMBA_B1.c
hci_qcomm_init_SOURCES += btqsocnvmefsmode.c
hci_qcomm_init_SOURCES += btqsocnvmplatform_linux.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BAHAMA_B0.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BAHAMA_A0.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4025_B3_19P2Mhz.c
hci_qcomm_init_SOURCES += bt_qsoc_nvm_BTS4025_B3_32Mhz.c
hci_qcomm_init_SOURCES += btqsocnvmtags.c
hci_qcomm_init_SOURCES += btqsocnvmprsr.c
hci_qcomm_init_SOURCES += btqsocnvmutils.c

hci_qcomm_init_LDFLAGS := -lpthread
hci_qcomm_init_LDFLAGS += -lrt
hci_qcomm_init_LDFLAGS += -lcutils
hci_qcomm_init_LDADD = libbtnv.la

if USE_GLIB
 hci_qcomm_init_CFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 hci_qcomm_init_CPPFLAGS += -DUSE_GLIB @GLIB_CFLAGS@
 hci_qcomm_init_LDFLAGS += @GLIB_LIBS@
endif

SUBDIRS = wcnss_filter

BTNVTOOL_CPPFLAGS = -DBT_QSOC_GET_ITEMS_FROM_NV \
    -I ${WORKSPACE}/wlan-proprietary/ftm/ \
    -I ${WORKSPACE}/qmi-framework/inc/ \
    -I ${WORKSPACE}/qmi/inc/ \
    -I ${WORKSPACE}/qmi/services/

if USE_GLIB
 btnvtool_CFLAGS = $(AM_CFLAGS) -fPIC -DUSE_GLIB @GLIB_CFLAGS@
 btnvtool_CPPFLAGS =$(BTNVTOOL_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
 btnvtool_LDFLAGS = -shared @GLIB_LIBS@
else
 btnvtool_CFLAGS = $(AM_CFLAGS) -fPIC
 btnvtool_CPPFLAGS =$(BTNVTOOL_CPPFLAGS)
 btnvtool_LDFLAGS = -shared
endif

requiredlibs = -lpthread
library_includedir = $(pkgincludedir)\
library_include_HEADERS = $(h_sources)
bin_PROGRAMS += btnvtool
btnvtool_CC = @CC@
btnvtool_SOURCES = btnvtool.cpp
btnvtool_SOURCES += bt_qmi_dms_client.c
btnvtool_LDADD = libbtnv.la -lqmiservices -lqmi_cci
