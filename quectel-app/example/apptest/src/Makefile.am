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

AM_CPPFLAGS = -fpie -pie \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat


requiredlibs = -lgthread-2.0 \
		 -lglib-2.0 -lpthread 


library_includedir = $(pkgincludedir)

apptest_sources = apptest.c

#QCMAP_CLI_h_sources =

bin_PROGRAMS = apptest 
apptest_CC = @CC@
apptest_SOURCES = $(apptest_sources)
#if USE_GLIB
apptest_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) #@GLIB_CFLAGS@
apptest_LDFLAGS = -lpthread $(XMLLIB_LIBS) #@GLIB_LIBS@
apptest_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) #@GLIB_CFLAGS@
#else
apptest_CFLAGS = $(AM_CFLAGS) -fPIC
apptest_LDFLAGS = -lpthread $(XMLLIB_LIBS)
apptest_CPPFLAGS = $(AM_CPPFLAGS)
#endif
apptest_LDADD = $(requiredlibs)
