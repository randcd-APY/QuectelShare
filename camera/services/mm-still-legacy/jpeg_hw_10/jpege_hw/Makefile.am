CURRENT_DIR = $(top_srcdir)/jpeg_hw_10/jpege_hw

AM_CFLAGS =  \
            -I$(CURRENT_DIR)/inc \
            -I$(top_srcdir)/jpeg_hw_10/common
AM_CFLAGS += -DUSE_ION



#Compile Shared library libjpegehw.so
lib_LTLIBRARIES = libjpegehw.la
libjpegehw_la_SOURCES = src/jpege_lib.c \
                        src/jpege_lib_hw.c \
                        src/jpege_app_util_mmap.c \
                        src/jpege_app_calc_param.c
libjpegehw_la_CFLAGS = $(AM_CFLAGS) -include stdint.h
libjpegehw_la_LIBADD = -lcutils -llog

h_sources = $(CURRENT_DIR)/inc/jpege_app_calc_param.h \
           $(CURRENT_DIR)/inc/jpege_app_util_mmap.h \
           $(CURRENT_DIR)/inc/jpege_lib.h \
           $(CURRENT_DIR)/inc/jpege_lib_common.h

pkgincludedir = $(includedir)/mm-still-legacy/jpeg_hw_10
library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)
