AM_CFLAGS = -Wall -Werror -g

AM_CFLAGS += $(LIBNL_CFLAGS)

requiredlibs = -lpthread  $(LIBNL_LIBS)

csm_src = csm.c

csm_SOURCES = $(csm_src)
csm_LDADD = $(requiredlibs)

bin_PROGRAMS = csm

etcdir = /etc
etc_SCRIPTS = csm.conf
