AM_CFLAGS = -Wall \
            -Wundef \
            -Wstrict-prototypes \
            -fno-inline \
            -fno-short-enums \
            -fpic

ACLOCAL_AMFLAGS = -I m4


AM_CPPFLAGS = -D__packed__= \
              -DFEATURE_LINUX \
              -DFEATURE_NATIVELINUX \
              -DFEATURE_LE_DIAG \
              -DMULTI_PROCESS_CONFIG \
	            $(QMIFRAMEWORK_CFLAGS) \
	            $(DIAG_FLAGS) \
              -I../server/utilities/bit_field/ \
              -I../server/utilities/list/ \
              -I../server/utilities/log/ \
              -I../server/utilities/memory/ \
              -I../server/utilities/synchronization/ \
              -I../server/utilities/control/ \
              -I../server/utilities/timer/ \
              -I../server/utilities/

c_sources = ./mcm_sim_test.c \
            ../server/utilities/bit_field/util_bit_field.c \
            ../server/utilities/list/util_list.c \
            ../server/utilities/memory/util_memory.c \
            ../server/utilities/log/util_log.c \
            ../server/utilities/synchronization/util_synchronization.c


requiredlibs = -lm \
               -lpthread \
               ../client/libmcm.la \
               $(QMIFRAMEWORK_LIBS)

bin_PROGRAMS = uim_test_client
testIoE_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
uim_test_client_SOURCES = $(c_sources)
uim_test_client_LDADD = $(requiredlibs)

EXTRA_DIST = $(pkgconfig_DATA)
