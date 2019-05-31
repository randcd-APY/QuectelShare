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

AM_CPPFLAGS = -D_packed_=\
	-I$(WORKSPACE)/quectel-core/ql-peripheral/ql-power/include

library_includedir = $(pkgincludedir)

powertest_sources = power_test.c

bin_PROGRAMS = powertest

powertest_SOURCES = $(powertest_sources)

powertest_CC = @CC@
powertest_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) #@GLIB_CFLAGS@
powertest_CPPFLAGS = -DUSE_GLIB $(AM_CPPFLAGS) #@GLIB_CFLAGS@

powertest_LDADD =  -lql_power
