
ACLOCAL_AMFLAGS = -I m4

brary_includedir = $(pkgincludedir)
lib_LTLIBRARIES = libmmi_sensor.la
libmmi_sensor_la_CC = @CC@
libmmi_sensor_la_CPPFLAGS = -O2 -g -Wno-all -Wno-error -Wno-unused-parameter -I./inc -I$(WORKSPACE)/system/core/include $(XML2_CFLAGS) -std=c++11 -DLINUX_ENABLED $(DIAG_CFLAGS) -I../../libmmi
libmmi_sensor_la_LDFLAGS = libcutils libutils libc libc++ ../../libmmi/libmmi.la -avoid-version
# source to compile

libmmi_sensor_la_SOURCES = sensor.cpp

if USE_GLIB
libmmi_sensor_la_CPPFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
libmmi_sensor_la_LDFLAGS += $(GLIB_LIBS)
endif

if NO_UI
libmmi_sensor_la_CPPFLAGS +=  -DNO_UI
endif

if ANDROID
libmmi_sensor_la_CPPFLAGS +=  -DANDROID
endif

if USE_LIBMINIUI
libmmi_sensor_la_CPPFLAGS +=  -DUSE_LIBMINIUI
endif
