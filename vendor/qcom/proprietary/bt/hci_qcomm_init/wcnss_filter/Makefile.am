# sources and intermediate files are separated

AM_CFLAGS = -Wall \
    -Wundef \
    -Wstrict-prototypes \
    -Wno-trigraphs \
    -std=c99 \
    -DWCNSS_IBS_ENABLED \
    -DWAKE_LOCK_ENABLED \
    -DDIAG_ENABLED \
    -DLOG_BT_ENABLE

AM_CPPFLAGS= \
    -Iinc/ \
    -I$(WORKSPACE)/diag/include \
    -I$(WORKSPACE)/system/core/include

# HAL layer
c_sources = \
    src/main.c \
    src/wcnss_ibs.c \
    src/bt_qxdmlog.c \
    src/hw_cherokee.c \
    src/uart_utils.c \
    src/utils.c \
    src/bt_vendor_persist.cpp

if USE_GLIB
 wcnssfilter_CFLAGS = $(AM_CFLAGS) -fPIC -D_GNU_SOURCE -DUSE_GLIB @GLIB_CFLAGS@
 wcnssfilter_LDFLAGS = @GLIB_LIBS@
else
 wcnssfilter_CFLAGS = $(AM_CFLAGS) -fPIC -D_GNU_SOURCE
endif

if MDM_ROME
    AM_CFLAGS += -DBT_SOC_TYPE_ROME
    AM_CPPFLAGS += -DBT_SOC_TYPE_ROME
else
if MDM_PRONTO
    AM_CFLAGS += -DHCI_USE_MCT
    AM_CPPFLAGS += -DHCI_USE_MCT
endif
endif

if MDM_CHEROKEE
    AM_CFLAGS += -DBT_SOC_TYPE_CHEROKEE
    AM_CPPFLAGS += -DBT_SOC_TYPE_CHEROKEE
endif

library_includedir = $(pkgincludedir)
bin_PROGRAMS = wcnssfilter
wcnssfilter_CC = @CC@
wcnssfilter_SOURCES = $(c_sources)

wcnssfilter_LDADD = -ldl -lrt -lresolv -lpthread -ldiag
