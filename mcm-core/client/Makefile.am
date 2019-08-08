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
        -I./core/ \
        -I./../ipc \
        -I./../mcm_log_util

c_sources =     core/mcm_client_multi_process_qmi.c \
                core/mcm_qmi_client.c \
                core/mcm_client.c  \
                core/mcm_client_internal.c \
                core/mcm_utils.c \
                ../ipc/mcm_dm_v01.c \
                ../ipc/mcm_nw_v01.c \
                ../ipc/mcm_voice_v01.c \
                ../ipc/mcm_sms_v01.c \
                ../ipc/mcm_common_v01.c \
                ../ipc/mcm_ril_v01.c \
                ../ipc/mcm_data_v01.c \
                ../ipc/mcm_mobileap_v01.c \
                ../ipc/mcm_atcop_v01.c \
                ../ipc/mcm_sim_v01.c \
                ../ipc/mcm_loc_v01.c \
                ../ipc/mcm_client_v01.c


library_includedir = $(pkgincludedir)

requiredlibs = -lm \
               -lpthread \
                $(QMIFRAMEWORK_LIBS) \
                ../mcm_log_util/libmcm_log_util.la


lib_LTLIBRARIES = libmcm.la
libmcm_la_CC = @CC@
libmcm_la_SOURCES = $(c_sources)
libmcm_la_LIBADD = $(requiredlibs)
libmcm_la_CPPFLAGS = -Dstrlcat=g_strlcat -Dstrlcpy=g_strlcpy $(AM_CPPFLAGS)
EXTRA_DIST = $(pkgconfig_DATA)

