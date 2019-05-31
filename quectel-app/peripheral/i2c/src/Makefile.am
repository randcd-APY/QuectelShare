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
        -fpie -pie \
	-I$(WORKSPACE)/quectel-core/ql-peripheral/ql-i2c/include \
        -Dstrlcpy=g_strlcpy \
        -Dstrlcat=g_strlcat


#####################QCMAP_Client##################
requiredlibs =  -lql_i2c

library_includedir = $(pkgincludedir)

i2ctest_sources = i2c_test.c

#QCMAP_CLI_h_sources =

bin_PROGRAMS = i2ctest
i2ctest_CC = @CC@
i2ctest_SOURCES = $(i2ctest_sources)
#if USE_GLIB
i2ctest_CFLAGS = -DUSE_GLIB $(AM_CFLAGS) #@GLIB_CFLAGS@
i2ctest_LDFLAGS = $(XMLLIB_LIBS) #@GLIB_LIBS@
i2ctest_CPPFLAGS = -DUSE_GLIB $(AM_CFLAGS) $(AM_CPPFLAGS) #@GLIB_CFLAGS@
#else
#Alpha_CFLAGS = $(AM_CFLAGS) -fPIC
#Alpha_LDFLAGS = -lpthread $(XMLLIB_LIBS)
#Alpha_CPPFLAGS = $(AM_CPPFLAGS)
#endif
i2ctest_LDADD = $(requiredlibs)
