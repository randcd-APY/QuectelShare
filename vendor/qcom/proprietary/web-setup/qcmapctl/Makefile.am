AM_CFLAGS = -Wall -Werror -Wextra

AM_CXXFLAGS = \
	$(DATA_CFLAGS) \
	$(QMI_CFLAGS) \
	$(QMI_FRAMEWORK_CFLAGS) \
	$(DSUTILS_CFLAGS)

if DEBUG
AM_CFLAGS += -DCONFIG_DEBUG -g
else
AM_CFLAGS += -O2 -g
endif

AM_CXXFLAGS += $(AM_CFLAGS)

requiredlibs = \
	-lqcmap_client \
	$(QMI_LIBS) \
	$(QMI_FRAMEWORK_LIBS) \
	$(DSUTILS_LIBS)

c_sources = \
	Main.cpp

qcmapctl_SOURCES = $(c_sources)
qcmapctl_LDADD = $(requiredlibs)

sbin_PROGRAMS = qcmapctl
