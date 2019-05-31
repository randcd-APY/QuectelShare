AM_CFLAGS = -Wundef \
            -Wstrict-prototypes \
            -Wno-trigraphs \
            -g -O0 \
            -fno-inline \
            -fno-short-enums \
            -fpic \
            $(LIBSELINUX_CFLAGS) \
            -I$(WORKSPACE)/system/core/include \
            -I$(WORKSPACE)/system/core/fs_mgr/include \
            -I$(WORKSPACE)/system/extras/ext4_utils \
            -D_GNU_SOURCE \
            -D_LARGEFILE64_SOURCE \
            -I..

AM_CPPFLAGS = \
              -DUSE_LE_MODE \
              -I.\
              -I../otafault \
              -I.. \
               $(LIBSELINUX_CFLAGS) \
              -I$(WORKSPACE)/system/core/base/include \
              -I$(WORKSPACE)/system/core/include \
              -I$(WORKSPACE)/system/core/fs_mgr/include \
              -I$(WORKSPACE)/system/extras/ext4_utils \
              -I$(WORKSPACE)/OTA/system/core/base/include \
              -std=c++11

AM_LDFLAGS = -shared -version-info 1:0:0
c_sources = install.cpp \
            blockimg.cpp \
            updater.cpp

library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)

requiredlibs = -lm \
               -lpthread \
               -lz \
               -lcutils \
               -lselinux \
               -lext4_utils \
               -lcrypto \
               -lrecovery_updater_msm \
               ../mtdutils/libmtdutils.la \
               ../edify/libedify.la \
               ../minzip/libminzip.la \
               ../applypatch/libapplypatch.la

bin_PROGRAMS = updater
if USE_GLIB
updater_SOURCES = $(c_sources) $(h_sources)
updater_CFLAGS = $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
updater_CPPFLAGS = $(AM_CPPFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
updater_LDADD = $(requiredlibs) @GLIB_LIBS@
else
updater_SOURCES = $(c_sources) $(h_sources)
updater_CFLAGS = $(AM_CFLAGS)
updater_CPPFLAGS = $(AM_CPPFLAGS)
updater_LDADD = $(requiredlibs)
endif

if TARGET_SUPPORTS_AB
updater_CPPFLAGS += -DTARGET_SUPPORTS_AB
updater_LDADD += -labctl
endif
