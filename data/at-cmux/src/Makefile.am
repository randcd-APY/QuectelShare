AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat \
        -lrt

AM_CXXFLAGS = \
        $(AM_CFLAGS) \
        -fpermissive

AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DAEEINTERFACE_CPLUSPLUS \
        -DFEATURE_SMD \
        -DFEATURE_DIAG_MP_MASTER_APPS \
        -DFEATURE_DS_LINUX_ANDROID \
        -DFEATURE_DSS_LINUX \
        -DFEATURE_LINUX \
        -DFEATURE_MULTIPROCESSOR \
        -DFEATURE_DSUTILS_OFFTARGET \
        -DFEATURE_NO_REX \
        -DFEATURE_XMLLIB \
        -I$(top_srcdir)/dsutils/inc \
        -I$(top_srcdir)/netmgr/inc \
        -I../inc \
        -I$(top_srcdir)/dsutils/ \
        -I../../dsutils/inc/ \
        $(DIAG_CFLAGS) \
                $(QMIFRAMEWORK_CFLAGS) \
        $(QMI_CFLAGS) \
                $(DSUTILS_CFLAGS) \
                $(XMLLIB_CFLAGS) \
        $(CFLAGS) \
                -Dstrlcpy=g_strlcpy \
                -Dstrlcat=g_strlcat

#####################DS_MUX##################
bin_includedir = $(pkgincludedir)
bin_include_HEADERS = ../inc/ds_mux_ipc.h

init_ddir = ${sysconfdir}/init.d
init_d_SCRIPTS = start_at_cmux_le

DS_MUX_sources = ds_mux_main.c ds_mux_smd_tty.c ds_mux_uart_tty.c ds_mux_ipc.c ds_mux.c ds_mux_utils.c ds_mux_api.c ds_mux_io.c

DS_MUX_h_sources = ds_mux_utils.h ds_mux_main.h ds_mux_ipc.h ds_mux.h ds_mux_types.h ds_mux_api.h ds_mux_types.h ds_mux_io.h

bin_PROGRAMS = DS_MUX
DS_MUX_CC = @CC@
DS_MUX_SOURCES = $(DS_MUX_sources)
if USE_GLIB
DS_MUX_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
DS_MUX_LDFLAGS = -lpthread $(DSUTILS_LIBS) $(XMLLIB_LIBS) $(DIAG_LIBS) @GLIB_LIBS@
DS_MUX_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@
else
DS_MUX_CFLAGS = $(AM_CFLAGS) $(DIAG_CFLAGS) -fPIC
DS_MUX_LDFLAGS = -lpthread $(DSUTILS_LIBS) $(DIAG_LIBS) $(XMLLIB_LIBS)
DS_MUX_CPPFLAGS = $(AM_CPPFLAGS)
endif
