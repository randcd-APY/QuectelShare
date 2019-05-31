AM_CFLAGS = -Wall

if DEBUG
AM_CFLAGS += -g
else
AM_CFLAGS += -O2
endif

AM_CFLAGS += -Wno-unused-parameter -Wno-int-to-pointer-cast \
	     -Wno-maybe-uninitialized -Wno-parentheses \
	     -I $(top_srcdir)/uapi/linux/ \
	     -I $(top_srcdir)/target/inc \
	     $(GLIB_CFLAGS) -Dstrlcpy=g_strlcpy \
	     $(CLD80211_LIB_CFLAGS)

spectraltool_SOURCES = spectraltool.c
spectraltool_CFLAGS = ${AM_CFLAGS} ${LIBNL_CFLAGS} -DCONFIG_CLD80211_LIB
spectraltool_LDADD = -lpthread ${LIBNL_LIBS} $(CLD80211_LIB_LIBS) @GLIB_LIBS@
bin_PROGRAMS = spectraltool
