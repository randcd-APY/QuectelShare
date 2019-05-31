ACLOCAL_AMFLAGS = -I m4

## build libcamera ##
lib_LTLIBRARIES = libcamera.la

include_HEADERS = inc/camera.h
include_HEADERS += inc/camera_parameters.h

if USE_QMMF
  libcamera_la_SOURCES  = src/qmmf_sdk/qmmf_camera.cpp
  libcamera_la_SOURCES += src/qmmf_sdk/camera_parameters.cpp

  libcamera_la_CXXFLAGS  = -fPIC
  libcamera_la_CXXFLAGS += -I$(srcdir)/qmmf_sdk/src/ 
  libcamera_la_CXXFLAGS += -I$(srcdir)/inc/

  libcamera_la_CXXFLAGS += -I$(SYSROOT)/usr/include/mm-core 
  libcamera_la_CXXFLAGS += "-D_HAL3_CAMERA_"
  libcamera_la_CPPFLAGS = -I$(CAMERAHAL_CFLAGS) -Werror
  libcamera_la_LDFLAGS  = -lutils -lcutils -llog -lqmmf_recorder_client
  
  ## build camera-test ##
  bin_PROGRAMS = camera-test

  camera_test_SOURCES = test/camera_test.cpp

  camera_test_CXXFLAGS = -I$(srcdir)/inc/
  camera_test_CXXFLAGS += "-D_HAL3_CAMERA_"
  camera_test_CXXFLAGS += -Werror

  camera_test_LDFLAGS  = -lutils -lcutils -llog
  camera_test_LDADD = libcamera.la -lpthread

endif


if USE_CAMHAL1

  libcamera_la_SOURCES  = src/camera_hal1/qcamera2.cpp
  libcamera_la_SOURCES += src/camera_hal1/camera_memory.cpp
  libcamera_la_SOURCES += src/camera_hal1/camera_parameters.cpp

  libcamera_la_CXXFLAGS  = -fPIC
  libcamera_la_CXXFLAGS += -include stdint.h
  
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../work-shared/msm8909/android_compat_build_artifacts/target/product/msm8909/obj/KERNEL_OBJ/usr/include
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../work-shared/msm8909/android_compat_build_artifacts/target/product/msm8909/obj/KERNEL_OBJ/usr/include/media


  libcamera_la_CXXFLAGS += -I$(srcdir)/src/camera_hal1
  libcamera_la_CXXFLAGS += -I$(srcdir)/inc

  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../sysroots/msm8909-robot/usr/include/camera
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../sysroots/msm8909-robot/usr/include/libstagefrighthw
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../../../../camera/lib/QCamera2/HAL
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../../../../camera/lib/QCamera2/util
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../../../../camera/lib/QCamera2/stack/common
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../../../../camera/lib/mm-image-codec/qomx_core
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../../../../camera/lib/mm-image-codec/qexif
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../../../../sysroots/msm8909-robot/usr/include/camera
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../../../../sysroots/msm8909-robot/usr/include/libstagefrighthw
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../../rb-camera/0.1-r0/lib/QCamera2/HAL
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../../rb-camera/0.1-r0/lib/QCamera2/util
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../../rb-camera/0.1-r0/lib/QCamera2/stack/common
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../../rb-camera/0.1-r0/lib/mm-image-codec/qomx_core
  libcamera_la_CXXFLAGS += -I$(srcdir)/../../../../../../../rb-camera/0.1-r0/lib/mm-image-codec/qexif  

  libcamera_la_CPPFLAGS = -I$(CAMERAHAL_CFLAGS) -Wno-error 
  
  libcamera_la_LDFLAGS = -shared
  libcamera_la_LIBADD = -ldl -lrt -lhardware -lmmcamera_interface -lmmjpeg_interface -lcamera_client -lmm-qcamera


  ## build camera-test ##
  bin_PROGRAMS = camera-test

  camera_test_SOURCES = test/camera_test.cpp

  camera_test_CXXFLAGS = -I$(srcdir)/inc
  camera_test_CXXFLAGS += -I$(srcdir)/src/camera_hal1
  
  camera_test_LDFLAGS  = -lutils -lcutils -llog -lmm-qcamera -lmmcamera_interface -lmmjpeg_interface -lcamera -lhardware
  camera_test_LDADD = libcamera.la -lpthread

endif



