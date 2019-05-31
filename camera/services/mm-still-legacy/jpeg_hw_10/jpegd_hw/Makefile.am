CURRENT_DIR = $(top_srcdir)/jpeg_hw_10/jpegd_hw

AM_CFLAGS = -D_ANDROID_ \
            -Werror \
            -I$(CURRENT_DIR)/inc \
            -I$(top_srcdir)/jpeg2/inc \
            -I$(CAMERAHAL_PATH)
AM_CFLAGS += -DUSE_ION


# Compile Shared library libjpegdhw.so
lib_LTLIBRARIES = libjpegdhw.la
libjpegdhw_la_SOURCES = src/jpegd_lib.c \
                        src/jpegd_lib_hw.c
libjpegdhw_la_CFLAGS = $(AM_CFLAGS) -include stdint.h
libjpegdhw_la_LIBADD = -lcutils -llog

h_sources = $(CURRENT_DIR)/inc/jpegd_lib.h
pkgincludedir = $(includedir)/mm-still-legacy/jpeg_hw_10
library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)
