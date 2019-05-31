
AM_CFLAGS = -Wundef -Wstrict-prototypes -Wno-trigraphs -std=c++11
AM_CFLAGS+= -I./inc -I$(WORKSPACE)/display/display-hal/include
AM_CFLAGS+= -DGBM_DEBUG -DALLOCATE_SURFACE_BO_AT_CREATION -DBUILD_HAS_WAYLAND_SUPPORT -DVENUS_COLOR_FORMAT

AM_CPPFLAGS = -Wundef -Wstrict-prototypes -Wno-trigraphs -std=c++11
AM_CPPFLAGS+= -I./inc -I$(WORKSPACE)/display/display-hal/include
AM_CPPFLAGS+= -DGBM_DEBUG -DALLOCATE_SURFACE_BO_AT_CREATION -DBUILD_HAS_WAYLAND_SUPPORT -DVENUS_COLOR_FORMAT

lib_LTLIBRARIES  = libgbm.la
libgbm_la_CFLAGS = $(AM_CFLAGS)
libgbm_la_CPPFLAGS = $(AM_CFLAGS)
libgbm_la_LDFLAGS = -shared -avoid-version -lm -ldl -lwayland-server -lwayland-client
libgbm_la_SOURCES = src/gbm.c \
                    src/msmgbm_camera.c \
                    src/msmgbm_video.c \
                    src/msmgbm_common.c \
                    src/msmgbm_platform_wrapper.cpp \
                    src/msmgbm_adreno_utils.cpp \
                    src/msmgbm_mapper.cpp

if ENABLE_COMPILEWITHDRM
libgbm_la_SOURCES += src/msmgbm.c
else
libgbm_la_SOURCES += src/msmgbm_fb.c
endif

bin_PROGRAMS = gbmtest
gbmtest_SOURCES = test/gbmtest.c
gbmtest_LDADD = libgbm.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gbm.pc
EXTRA_DIST = $(pkgconfig_DATA)
