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
        -fpermissive

AM_CPPFLAGS = -fpie -pie \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat

requiredlibs = -lrt \
		 -ldl

library_includedir = $(pkgincludedir)

btconfig_sources = btconfig.c

bin_PROGRAMS = btconfig
btconfig_CC = @CC@
btconfig_SOURCES = $(btconfig_sources)
btconfig_CFLAGS = $(AM_CFLAGS) -fPIC
btconfig_LDFLAGS = -lpthread
btconfig_CPPFLAGS = -fPIC $(AM_CPPFLAGS) 
btconfig_LDADD = $(requiredlibs)
