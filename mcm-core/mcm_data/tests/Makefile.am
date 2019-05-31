AM_CFLAGS = -Wundef \
        -Wno-trigraphs \
        -fno-inline \
        -fno-short-enums \
        -fpic

#refer to Androik.mk for staging header files
AM_CPPFLAGS = -D__packed__= \
        -DIMAGE_APPS_PROC \
        -DFEATURE_Q_NO_SELF_QPTR \
        -DAEEINTERFACE_CPLUSPLUS \
        -DFEATURE_SMD \
        -DFEATURE_DIAG_MP_MASTER_APPS \
        -DFEATURE_DATA_LINUX_LE \
        -DFEATURE_MULTIPROCESSOR \
        -DFEATURE_DSUTILS_OFFTARGET \
        -DFEATURE_NO_REX \
        $(DSUTILS_CFLAGS) \
        -I../../ipc/

library_includedir = $(pkgincludedir)

requiredlibs = ../../client/libmcm.la

testsdir = $(prefix)/tests

mcm_data_test_CC = @CC@
mcm_data_test_SOURCES = mcm_data_test.c
if USE_GLIB
mcm_data_test_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) @GLIB_CFLAGS@
mcm_data_test_LDFLAGS = -lpthread $(DSUTILS_LIBS) -lqmi $(DIAG_LIBS) -lqmi_cci @GLIB_LIBS@ -shared -version-info 1:0:0
else
mcm_data_test_CFLAGS = $(AM_CFLAGS) $(DIAG_CFLAGS) -fPIC
mcm_data_test_LDFLAGS = -lpthread $(DSUTILS_LIBS) -lqmi $(DIAG_LIBS) -lqmi_cci -shared -version-info 1:0:0
endif
mcm_data_test_CPPFLAGS = $(AM_CPPFLAGS)
mcm_data_test_LDADD = $(requiredlibs)

tests_PROGRAMS = mcm_data_test
