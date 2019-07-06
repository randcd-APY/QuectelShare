AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -fpie -pie
 
AM_CPPFLAGS = -D__packed__= \
	      -fpie -pie \
	      -I$(WORKSPACE)/quectel-core/ql-mcm-api/inc \
	      -I$(WORKSPACE)/qmi/core/lib/inc \
	      -I$(WORKSPACE)/mcm-api/api \
	      -I../inc

requiredlibs =  -lrt \
		-lql_mcm_api \
		$(MCMCORE_LIBS)	

name = MCM_API_TEST
bin_PROGRAMS = $(name)
MCM_API_TEST_SOURCES = test_sim.c test_main.c test_data.c test_mobap.c test_voice.c test_atcop.c test_dm.c test_nw.c

MCM_API_TEST_CC = @CC@
MCM_API_TEST_CFLAGS = -DUSE_GLIB $(AM_CFLAGS)
MCM_API_TEST_LDFLAGS = -lpthread $(XMLLIB_LIBS)
MCM_API_TEST_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS)
MCM_API_TEST_LDADD = $(requiredlibs)
