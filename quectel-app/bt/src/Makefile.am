AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -fpie -pie \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat \
		-I$(WORKSPACE)/quectel-core/ql-bt/inc

AM_CXXFLAGS = \
        -fpermissive

AM_CPPFLAGS = -fpie -pie \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat

requiredlibs = -lgthread-2.0 \
		 -lglib-2.0 -lpthread 

library_includedir = $(pkgincludedir)

BT_TEST_SERVER_sources = server.c

bin_PROGRAMS = BT_TEST_SERVER 
BT_TEST_SERVER_CC = @CC@
BT_TEST_SERVER_SOURCES = $(BT_TEST_SERVER_sources)
BT_TEST_SERVER_CFLAGS = $(AM_CFLAGS) -fPIC
BT_TEST_SERVER_LDFLAGS = -lpthread -lql_bt 
BT_TEST_SERVER_CPPFLAGS = -fPIC $(AM_CPPFLAGS) 
BT_TEST_SERVER_LDADD = $(requiredlibs)


BT_TEST_CLIENT_sources = client.c

bin_PROGRAMS += BT_TEST_CLIENT 
BT_TEST_CLIENT_CC = @CC@
BT_TEST_CLIENT_SOURCES = $(BT_TEST_CLIENT_sources)
BT_TEST_CLIENT_CFLAGS = $(AM_CFLAGS) -fPIC
BT_TEST_CLIENT_LDFLAGS = -lpthread -lql_bt 
BT_TEST_CLIENT_CPPFLAGS = -fPIC $(AM_CPPFLAGS) 
BT_TEST_CLIENT_LDADD = $(requiredlibs)
