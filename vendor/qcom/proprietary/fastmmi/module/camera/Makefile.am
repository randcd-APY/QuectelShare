ACLOCAL_AMFLAGS = -I m4

if !QSAP
if ARM
brary_includedir = $(pkgincludedir)
lib_LTLIBRARIES = libmmi_camera.la
libmmi_camera_la_CC = @CC@

# source to compile
libmmi_camera_la_SOURCES = camera_le.cpp

libmmi_camera_la_CPPFLAGS := -O2 -g -Wno-all -Wno-error -Wno-unused-parameter -I. -I$(WORKSPACE)/system/core/include $(XML2_CFLAGS) -std=gnu++0x -DLINUX_ENABLED $(DIAG_CFLAGS) -I../../libmmi

libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/external/libcxx/include
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/service/mm-camera/apps/appslib
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/service/mm-still/ipl/inc
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/service/mm-still/jpeg/inc
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/frameworks/native/include/media/openmax

if MSM8909
#libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/lib-legacy/libhardware/include
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/mm-camera-test/inc
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/mm-camera-interface/inc
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/lib-legacy/mm-image-codec/qexif
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/lib-legacy/mm-image-codec/qomx_core
else
#libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/lib/libhardware/include
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/lib/QCamera2/stack/common
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/lib/QCamera2/stack/mm-camera-test/inc
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/lib/QCamera2/stack/mm-camera-interface/inc
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/lib/mm-image-codec/qexif
libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/camera/lib/mm-image-codec/qomx_core
endif
libmmi_camera_la_LDFLAGS := -lcutils
libmmi_camera_la_LDFLAGS += ../../libmmi/libmmi.la -avoid-version
libmmi_camera_la_LDFLAGS += -ldl
libmmi_camera_la_LDFLAGS += -lmm-qcamera
libmmi_camera_la_LDFLAGS += -lmmcamera_interface

if USE_GLIB
libmmi_camera_la_CPPFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
libmmi_camera_la_LDFLAGS += $(GLIB_LIBS)
endif

if NO_UI
libmmi_camera_la_CPPFLAGS +=  -DNO_UI
#libmmi_camera_la_CPPFLAGS += -I $(WORKSPACE)/bootable/recovery/minui
#libmmi_camera_la_LDFLAGS += -lui
#libmmi_camera_la_LDFLAGS += -lgui
endif

if ANDROID
libmmi_camera_la_CPPFLAGS +=  -DANDROID
libmmi_camera_la_LDFLAGS += -landroid
endif

if USE_LIBMINIUI
libmmi_camera_la_CPPFLAGS +=  -DUSE_LIBMINIUI
endif

if MSM8909
libmmi_camera_la_CPPFLAGS +=  -DCAM_API_LEGACY
endif

endif
endif
