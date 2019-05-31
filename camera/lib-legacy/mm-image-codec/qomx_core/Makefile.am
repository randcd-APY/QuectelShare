ACLOCAL_AMFLAGS = -I m4

CURR_PATH = $(top_srcdir)/mm-image-codec/qomx_core

#libqomx_core.so
lib_LTLIBRARIES = libqomx_core.la

AM_CFLAGS = -g -O0 -Werror

AM_CPPFLAGS = -DLOGE=ALOGE
AM_CPPFLAGS += -I ../qexif
AM_CPPFLAGS += -I qomx_core
AM_CPPFLAGS += -I $(WORKSPACE)/frameworks/native/include/media/openmax

#h_sources = ../qexif/qexif.h
#h_sources += QOMX_JpegExtensions.h
#h_sources += QOMX_JpegExtensions.h

libqomx_core_la_SOURCES = qomx_core.c
libqomx_core_la_CFLAGS = $(AM_CFLAGS)
libqomx_core_la_CPPFLAGS = $(AM_CPPFLAGS)
libqomx_core_la_LDFLAGS = -shared
libqomx_core_la_LIBADD = -lcutils -ldl -llog

#pkgincludedir = $(includedir)/
#library_includedir = $(pkgincludedir)
#library_include_HEADERS = $(h_sources)
