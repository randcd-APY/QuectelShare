CURRENT_DIR = $(top_srcdir)/jpeg_hw_10/jpegdma_hw

AM_CFLAGS = -I$(CURRENT_DIR)/inc \
            -I$(top_srcdir)/jpeg_hw_10/common

if TARGET_USES_ION
AM_CFLAGS += -DUSE_ION
endif
if NEW_LOG_API
AM_CFLAGS += -DNEW_LOG_API
endif

#Compile Shared library libjpegdmahw.so
lib_LTLIBRARIES = libjpegdmahw.la
libjpegdmahw_la_SOURCES = src/jpegdma_lib.c \
                          src/jpegdma_lib_hw.c \
                          src/jpegdma_app_util_mmap.c
libjpegdmahw_la_CFLAGS = $(AM_CFLAGS) -include stdint.h
libjpegdmahw_la_LIBADD = -lcutils

h_sources = $(CURRENT_DIR)/inc/jpegdma_app_util_mmap.h \
	    $(CURRENT_DIR)/inc/jpegdma_lib.h \
	    $(CURRENT_DIR)/inc/jpegdma_lib_common.h

pkgincludedir = $(includedir)/mm-still/jpeg_hw_10
library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)
