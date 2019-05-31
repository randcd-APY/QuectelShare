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
	-I$(WORKSPACE)/quectel-core/ql-peripheral/ql-vibrator/include

library_includedir = $(pkgincludedir)

vibratortest_sources = vibrator_test.c

bin_PROGRAMS = vibratortest

vibratortest_SOURCES = $(vibratortest_sources)

vibratortest_CC = @CC@
vibratortest_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) #@GLIB_CFLAGS@
vibratortest_CPPFLAGS = -DUSE_GLIB $(AM_CPPFLAGS) #@GLIB_CFLAGS@

vibratortest_LDADD =  -lql_vibrator
