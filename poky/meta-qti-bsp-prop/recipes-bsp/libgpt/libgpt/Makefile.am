# Export header
libgptheaders_includedir=$(includedir)
libgptheaders_include_HEADERS := $(top_srcdir)/gpt.h

# Build libgpt.so
lib_LTLIBRARIES      := libgpt.la

libgpt_la_SOURCES    := gpt.cpp

libgpt_la_CPPFLAGS   := -I$(top_srcdir)/
libgpt_la_CPPFLAGS   += -DDBG_PRINTS
libgpt_la_CPPFLAGS   += -std=c++11 -Wall -Wno-unused-parameter
libgpt_la_CPPFLAGS   += -Wno-deprecated-declarations
