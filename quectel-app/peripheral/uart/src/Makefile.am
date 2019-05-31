AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -fpie -pie \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat
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
        -fpie -pie \
	-I$(WORKSPACE)/quectel-core/ql-peripheral/ql-uart/include \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat


#####################QCMAP_Client##################
requiredlibs =  -lql_uart

library_includedir = $(pkgincludedir)

uarttest_sources = uart_test.c

#QCMAP_CLI_h_sources =

bin_PROGRAMS = uarttest
uarttest_CC = @CC@
uarttest_SOURCES = $(uarttest_sources)
#if USE_GLIB
uarttest_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) #@GLIB_CFLAGS@
uarttest_LDFLAGS = $(XMLLIB_LIBS) #@GLIB_LIBS@
uarttest_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) #@GLIB_CFLAGS@
#else
#Alpha_CFLAGS = $(AM_CFLAGS) -fPIC
#Alpha_LDFLAGS = -lpthread $(XMLLIB_LIBS)
#Alpha_CPPFLAGS = $(AM_CPPFLAGS)
#endif
uarttest_LDADD = $(requiredlibs)
