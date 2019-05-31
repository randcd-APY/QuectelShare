CURRENT_DIR = $(top_srcdir)/mercury

AM_CFLAGS = -I$(CURRENT_DIR)/inc \
            -include stdint.h

if TARGET_USES_ION
AM_CFLAGS += -DUSE_ION
endif
if NEW_LOG_API
AM_CFLAGS += -DNEW_LOG_API
endif

# Compile shared library libmercury.so
lib_LTLIBRARIES = libmercury.la
libmercury_la_SOURCES = src/mercury_lib.c \
                        src/mercury_lib_hw.c
libmercury_la_LIBADD = -lcutils -llog
