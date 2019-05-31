# Build libabctl.so
lib_LTLIBRARIES        := libabctl.la

libabctl_la_SOURCES    := libabctl.cpp

libabctl_la_CPPFLAGS   := -I$(top_srcdir)/inc
libabctl_la_CPPFLAGS   += -std=c++11 -Wall -Wno-unused-parameter
libabctl_la_CPPFLAGS   += -Wno-deprecated-declarations
libabctl_la_LDFLAGS    := -lgpt

# Build abctl utility
root_sbindir           := "/sbin"
root_sbin_PROGRAMS     := abctl

abctl_SOURCES  := abctl.cpp

abctl_CPPFLAGS := -I$(top_srcdir)/inc
abctl_CPPFLAGS += -std=c++11 -Wall -Wno-unused-parameter
abctl_LDADD    := libabctl.la
