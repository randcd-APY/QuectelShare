# sources and intermediate files are separated

AM_CFLAGS = -Wall \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -Wno-unused-label \
        -std=c99 \
        -DBT_NV_SUPPORT

AM_CPPFLAGS = -Iinclude \
        -I${WORKSPACE}/system/bt/hci/include \
        -DBT_NV_SUPPORT

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

c_sources = \
        src/bt_vendor_qcom.c \
        src/hardware.c \
        src/hci_uart.c \
        src/hci_smd.c \
        src/hw_rome.c \
        src/hw_ar3k.c \
        src/bt_vendor_persist.cpp

if USE_GLIB
 libbt_vendor_la_CFLAGS = $(AM_CFLAGS) -fPIC -D_GNU_SOURCE -D__KERNEL__ -DUSE_GLIB @GLIB_CFLAGS@
 libbt_vendor_la_LDFLAGS = -shared @GLIB_LIBS@
else
 libbt_vendor_la_CFLAGS = $(AM_CFLAGS) -fPIC -D_GNU_SOURCE -D__KERNEL__
 libbt_vendor_la_LDFLAGS = -shared
endif

requiredlibs = -lpthread -lbtnv

library_includedir = $(pkgincludedir)\
library_include_HEADERS = $(h_sources)
lib_LTLIBRARIES = libbt-vendor.la
libbt_vendor_la_CC = @CC@
libbt_vendor_la_SOURCES = $(c_sources)
libbt_vendor_la_CPPFLAGS =$(AM_CPPFLAGS)
libbt_vendor_la_LIBADD = $(requiredlibs)
