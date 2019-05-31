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
	-I$(WORKSPACE)/quectel-core/ql-peripheral/ql-adc/include

library_includedir = $(pkgincludedir)

adctest_sources = adc_test.c

bin_PROGRAMS = adctest 

adctest_SOURCES = $(adctest_sources)

adctest_CC = @CC@
adctest_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) #@GLIB_CFLAGS@
adctest_CPPFLAGS = -DUSE_GLIB $(AM_CPPFLAGS) #@GLIB_CFLAGS@

adctest_LDADD = -lql_adc
