CURRENT_DIR = $(top_srcdir)/gemini

AM_CFLAGS = -I$(CURRENT_DIR)/inc \
             -include stdint.h

if TARGET_USES_ION
AM_CFLAGS += -DUSE_ION
endif
if NEW_LOG_API
AM_CFLAGS += -DNEW_LOG_API
endif

#Compile shared library libgemini
lib_LTLIBRARIES = libgemini.la
libgemini_la_SOURCES = src/gemini_lib.c \
                       src/gemini_lib_hw.c \
                       src/gemini_app_util_mmap.c \
                       src/gemini_app_calc_param.c \
                       src/gemini_inline.c
libgemini_la_LIBADD = -lcutils

#Compile test app test_gemini
bin_PROGRAMS = test_gemini
test_gemini_SOURCES = test/test_gemini.c \
                      test/gemini_app.c
test_gemini_LDADD = libgemini.la -lcutils -llog
