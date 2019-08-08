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
        -Dstrlcpy=g_strlcpy \
        $(QMIFRAMEWORK_CFLAGS) \
        $(DIAG_FLAGS) \
        -I./core/

c_sources = qmi_client_utils.c \
        qmi_simple_ril_core.c \
        qmi_simple_ril_suite.c \
        qmi_simple_ril_dm.c \
        qmi_simple_ril_nas.c \
        qmi_simple_ril_wms.c \
        qmi_simple_ril_voice.c \
        qmi_test_console.c \
        qmi_simple_ril_srv_mgr.c


requiredlibs = -lm \
        -lpthread \
        ../client/libmcm.la \
        $(QMIFRAMEWORK_LIBS)

bin_PROGRAMS = IoEConsoleClient 
testIoE_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
IoEConsoleClient_SOURCES = $(c_sources)
IoEConsoleClient_LDADD = $(requiredlibs) @GLIB_LIBS@
IoEConsoleClient_CPPFLAGS = $(AM_CPPFLAGS)

EXTRA_DIST = $(pkgconfig_DATA)
