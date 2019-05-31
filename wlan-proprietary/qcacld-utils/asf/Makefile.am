AM_CFLAGS = -Wno-error

if DEBUG
AM_CFLAGS += -DCONFIG_DEBUG -g
else
AM_CFLAGS += -O2
endif

AM_CFLAGS += -fno-short-enums

AM_CFLAGS += -DMDM_LE

AM_CXXFLAGS = $(AM_CFLAGS)

c_sources := src/aniAsfTimer.c
c_sources += src/aniAsfIpc.c
c_sources += src/aniAsfEvent.c
c_sources += src/aniAsfLog.c
c_sources += src/aniAsfProcessUtils.c
c_sources += src/aniAsfUtils.c
c_sources += src/aniAsfMisc.c
c_sources += src/aniAsfHashTable.c
c_sources += src/aniAsfDict.c
c_sources += src/aniAsfPacket.c
c_sources += src/aniAsfPortMap.c

libAniAsf_a_SOURCES = $(c_sources)
libAniAsf_a_CFLAGS  = $(AM_CFLAGS) -Iinc
libAniAsf_a_LDFLAGS = -lcutils

lib_LIBRARIES = libAniAsf.a

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = AniAsf.pc
EXTRA_DIST = $(pkgconfig_DATA)
