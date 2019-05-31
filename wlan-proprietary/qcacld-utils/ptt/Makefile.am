AM_CFLAGS = -Wno-error

if DEBUG
AM_CFLAGS += -DCONFIG_DEBUG -g
else
AM_CFLAGS += -O2
endif

AM_CFLAGS += -fno-short-enums
AM_CFLAGS +=  -D_GNU_SOURCE
AM_CFLAGS += -DWLAN_KD_READY_NOTIFIER
AM_CFLAGS += -DWIFI_DRIVER_MODULE_PATH
AM_CFLAGS += $(DIAG_CFLAGS)
AM_CFLAGS += $(ASF_CFLAGS)

AM_CFLAGS += -DMDM_LE

AM_CXXFLAGS = $(AM_CFLAGS)

requiredlibs = $(DIAG_LIBS) $(ASF_LIBS) -lcutils

c_sources = pttSocketApp.c

ptt_socket_app_SOURCES = $(c_sources)
ptt_socket_app_CFLAGS  = $(AM_CFLAGS) -std=gnu99 -I$(top_srcdir)/asf/inc
ptt_socket_app_LDADD = $(requiredlibs)
ptt_socket_app_LDADD += ../asf/libAniAsf.a
bin_PROGRAMS = ptt_socket_app

