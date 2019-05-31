# Automake script for WMS translation services library supporting QBI

noinst_LTLIBRARIES = libmbim_wmsts.la

libmbim_wmsts_la_CFLAGS = -Wall -Werror -fno-short-enums -fno-strict-aliasing
libmbim_wmsts_la_CPPFLAGS = $(MBIM_CFLAGS) -D_POSIX_C_SOURCE=200809L -Iinc -Iapi $(DIAG_CFLAGS)
libmbim_wmsts_la_LIBADD = -lpthread
libmbim_wmsts_la_LDFLAGS = -static
libmbim_wmsts_la_SOURCES = src/bit.c src/qw.c src/wmsts.c src/wmstscdma.c
