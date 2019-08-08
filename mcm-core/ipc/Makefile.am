# Makefile.am - Automake script for qcril_qmi
AM_CFLAGS = -Wall \
        -Wundef \
        -Wstrict-prototypes \
        -fno-inline \
        -fno-short-enums \
        -fpic

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -D__packed__= \
        -DFEATURE_LINUX \
        -DFEATURE_NATIVELINUX \
        -DFEATURE_LE_DIAG \
        -DMULTI_PROCESS_CONFIG \
        $(QMIFRAMEWORK_CFLAGS) \
        $(DIAG_FLAGS) \
        -I./ipc

c_sources =     ./mcm_dm_v01.c \
                ./mcm_nw_v01.c \
                ./mcm_voice_v01.c \
                ./mcm_sms_v01.c \
                ./mcm_common_v01.c \
                ./mcm_ril_v01.c \
                ./mcm_data_v01.c \
                ./mcm_mobileap_v01.c \
                ./mcm_atcop_v01.c \
                ./mcm_sim_v01.c \
                ./mcm_loc_v01.c \
                ./mcm_client_v01.c \
                ./mcm_data_srv_mgr_v01.c \
                ./mcm_atcop_srv_mgr_v01.c \
                ./mcm_mobileap_srv_mgr_v01.c \
                ./mcm_loc_srv_mgr_v01.c \
                ./mcm_ipc.c


library_includedir = $(pkgincludedir)

lib_LTLIBRARIES = libmcmipc.la
libmcmipc_la_CC = @CC@
libmcmipc_la_SOURCES = $(c_sources)
libmcmipc_la_LIBADD = -lpthread -lm $(QMIFRAMEWORK_LIBS)
libmcmipc_la_CPPFLAGS = $(AM_CPPFLAGS)
EXTRA_DIST = $(pkgconfig_DATA)

