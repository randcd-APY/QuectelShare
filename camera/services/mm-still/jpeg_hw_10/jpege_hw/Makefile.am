CURRENT_DIR = $(top_srcdir)/jpeg_hw_10/jpege_hw

AM_CFLAGS = -I$(CURRENT_DIR)/inc \
            -I$(top_srcdir)/jpeg_hw_10/common

if TARGET_ENABLE_INSTRUMENTATION
AM_CFLAGS += -DTARGET_ENABLE_INSTRUMENTATION
AM_CFLAGS += --coverage
endif
if BURST_LEN_16
AM_CFLAGS += -DBURST_LEN_16
endif
if TARGET_USES_ION
AM_CFLAGS += -DUSE_ION
endif
if NEW_LOG_API
AM_CFLAGS += -DNEW_LOG_API
endif

#Compile Shared library libjpegehw.so
lib_LTLIBRARIES = libjpegehw.la
libjpegehw_la_SOURCES = src/jpege_lib.c \
                        src/jpege_lib_hw.c \
                        src/jpege_app_util_mmap.c \
                        src/jpege_app_calc_param.c
libjpegehw_la_CFLAGS = $(AM_CFLAGS) -include stdint.h
libjpegehw_la_LIBADD = -lcutils

if TARGET_ENABLE_INSTRUMENTATION
libjpegehw_la_LDFLAGS = --coverage
libjpegehw_la_LIBADD += -lgcov
endif

h_sources = $(CURRENT_DIR)/inc/jpege_app_calc_param.h \
	    $(CURRENT_DIR)/inc/jpege_app_util_mmap.h \
	    $(CURRENT_DIR)/inc/jpege_lib.h \
	    $(CURRENT_DIR)/inc/jpege_lib_common.h

pkgincludedir = $(includedir)/mm-still/jpeg_hw_10
library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)
