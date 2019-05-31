AM_CFLAGS := -Wundef \
             -Wstrict-prototypes \
             -Wno-trigraphs \
             -g -O0 \
             -fno-inline \
             -fno-short-enums \
             -fpic \
             -D_GNU_SOURCE \
             -I..

AM_CPPFLAGS := \
              -DUSE_LE_MODE \
              -I. \
              -I.. \
              -I./include \
              $(LIBSELINUX_CFLAGS) \
              -I$(WORKSPACE)/system/core/include \
              -I$(WORKSPACE)/system/core/base/include \
              -I$(WORKSPACE)/system/core/fs_mgr/include \
              -I$(WORKSPACE)/OTA/system/core/base/include \
              -std=c++11 \
              -DENABLE_LEGACY_BOOTLOADER_MSG_UTILS \
              -Dstrlcpy=g_strlcpy \
              -Dstrlcat=g_strlcat

AM_LDFLAGS := -shared -version-info 1:0:0

c_sources := bootloader_message.cpp

library_includedir := $(pkgincludedir)

requiredlibs := -lbase

lib_LTLIBRARIES := libbootloader_message.la

if USE_GLIB
libbootloader_message_la_SOURCES := $(c_sources) $(h_sources)
libbootloader_message_la_CFLAGS := $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libbootloader_message_la_CPPFLAGS := $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libbootloader_message_la_LIBADD := $(requiredlibs) @GLIB_LIBS@
libbootloader_message_la_LDFLAGS := -all-static
else
libbootloader_message_la_SOURCES := $(c_sources) $(h_sources)
libbootloader_message_la_CFLAGS := $(AM_CFLAGS)
libbootloader_message_la_CPPFLAGS := $(AM_CPPFLAGS)
libbootloader_message_la_LIBADD := $(requiredlibs)
libbootloader_message_la_LDFLAGS := -all-static
endif
