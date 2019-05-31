AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
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
        -I../inc \
        $(QMI_CFLAGS) \
        $(DIAG_CFLAGS) \
        $(DSUTILS_CFLAGS) \
        $(XMLLIB_CFLAGS) \
        $(CFLAGS) \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat

requiredlibs = -lpthread -lrt
#####################qti_socksv5##################

qti_socksv5_sources   = qti_socksv5_pkt_factory.cpp  qti_socksv5_cfg.cpp  qti_socksv5_util.cpp
qti_socksv5_sources  += qti_socksv5_garbage_collector.cpp  qti_socksv5.cpp

qti_socksv5_h_sources  = qti_socksv5_pkt_factory.hpp  qti_socksv5_cfg.hpp  qti_socksv5_util.hpp
qti_socksv5_h_sources += qti_socksv5_garbage_collector.hpp  qti_socksv5_log_msg.h  qti_socksv5.hpp

bin_PROGRAMS = qti_socksv5

qti_socksv5_CC = @CC@
qti_socksv5_SOURCES = $(qti_socksv5_sources) $(qti_socksv5_h_sources)
if USE_GLIB
qti_socksv5_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
qti_socksv5_LDFLAGS = -lpthread $(DSUTILS_LIBS) $(XMLLIB_LIBS) $(DIAG_LIBS) @GLIB_LIBS@ -lpugixml
qti_socksv5_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) @GLIB_CFLAGS@  -std=gnu++11
else
qti_socksv5_CFLAGS = $(AM_CFLAGS)
qti_socksv5_LDFLAGS = -lpthread $(DSUTILS_LIBS) $(XMLLIB_LIBS) $(DIAG_LIBS) -lpugixml
qti_socksv5_CPPFLAGS = $(AM_CPPFLAGS) -std=gnu++11
endif
qti_socksv5_LDADD = $(requiredlibs)
