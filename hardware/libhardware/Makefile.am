
lib_LTLIBRARIES = libhardware.la

libhardware_la_SOURCES = hardware.c
libhardware_la_CFLAGS = -I$(top_srcdir)/include -DLINUX_ENABLED
libhardware_la_LDFLAGS = -lcutils -llog -ldl

# Export headers
h_sources  = $(top_srcdir)/include/hardware/hardware.h
h_sources += $(top_srcdir)/include/hardware/power.h
h_sources += $(top_srcdir)/include/hardware/qemu_pipe.h

if HAS_AUDIO
h_sources += $(top_srcdir)/include/hardware/audio.h
h_sources += $(top_srcdir)/include/hardware/sound_trigger.h
h_sources += $(top_srcdir)/include/hardware/audio_alsaops.h
h_sources += $(top_srcdir)/include/hardware/audio_effect.h
h_sources += $(top_srcdir)/include/hardware/audio_policy.h
endif

if HAS_CAMERA
h_sources += $(top_srcdir)/include/hardware/camera.h
h_sources += $(top_srcdir)/include/hardware/camera3.h
h_sources += $(top_srcdir)/include/hardware/camera_common.h
endif

if HAS_DISPLAY
h_sources += $(top_srcdir)/include/hardware/hwcomposer.h
h_sources += $(top_srcdir)/include/hardware/hwcomposer_defs.h
h_sources += $(top_srcdir)/include/hardware/gralloc.h
h_sources += $(top_srcdir)/include/hardware/fb.h
endif

if HAS_LOCATION
h_sources += $(top_srcdir)/include/hardware/gps.h
h_sources += $(top_srcdir)/include/hardware/fused_location.h
endif

if HAS_SENSORS
h_sources += $(top_srcdir)/include/hardware/sensors.h
endif

gralloc_h_sources = modules/gralloc/gralloc_priv.h

libhardware_includedir=$(includedir)/hardware/
libhardware_include_HEADERS = $(h_sources)

libhardware_gralloc_includedir=$(includedir)/libhardware/
libhardware_gralloc_include_HEADERS = $(gralloc_h_sources)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = hardware.pc
EXTRA_DIST = $(pkgconfig_DATA)
