# Makefile.am - Automake script for Logging
AM_CFLAGS = -Wall \
        -Wundef \
        -Wstrict-prototypes \
        -fno-inline \
        -fno-short-enums \
        -fpic -g

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -D__packed__= \
        -DFEATURE_LINUX \
        -DFEATURE_NATIVELINUX \
        -DFEATURE_LE_DIAG \
        -DMULTI_PROCESS_CONFIG \
        $(QMIFRAMEWORK_CFLAGS) \
        $(DIAG_FLAGS) \
        -I./../common \
        -I./../server/utilities/log

c_sources = mcm_ssr_util.c \
            ../server/utilities/log/util_log.c

library_includedir = $(pkgincludedir)

requiredlibs = -lm \
               -lpthread

lib_LTLIBRARIES = libmcm_ssr_util.la
libmcm_ssr_util_la_CC = @CC@
libmcm_ssr_util_la_SOURCES = $(c_sources)
libmcm_ssr_util_la_LIBADD = $(requiredlibs)
libmcm_ssr_util_la_CPPFLAGS = -Dstrlcat=g_strlcat -Dstrlcpy=g_strlcpy $(AM_CPPFLAGS)
EXTRA_DIST = $(pkgconfig_DATA)

