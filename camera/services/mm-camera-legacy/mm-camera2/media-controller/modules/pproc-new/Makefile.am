#ACLOCAL_AMFLAGS = -I m4

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/pproc-new
MM2_PATH = $(top_srcdir)/mm-camera2
BUILD_PATH = $(top_srcdir)/../../../build/mm-camera2

#libmmcamera2_pproc_modules.so
lib_LTLIBRARIES = libmmcamera2_pproc_modules.la

libmmcamera2_pproc_modules_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
libmmcamera2_pproc_modules_la_CFLAGS += -DMSM_CAMERA_GCC
libmmcamera2_pproc_modules_la_CFLAGS += -fPIC
#libmmcamera2_pproc_modules_la_CFLAGS += -DCAMERA_FEATURE_CAC
libmmcamera2_pproc_modules_la_CFLAGS += -DPPROC_METADATA_HEADER_VERSION=0x0
libmmcamera2_pproc_modules_la_CFLAGS += -include stdint.h
libmmcamera2_pproc_modules_la_CFLAGS += -DLOGE=ALOGE
libmmcamera2_pproc_modules_la_CFLAGS += -include utils/Log.h
libmmcamera2_pproc_modules_la_CFLAGS += -D_ANDROID_

#if MSM8909
libmmcamera2_pproc_modules_la_CFLAGS += -DCAMERA_FEATURE_WNR_SW #kyle delete
#libmmcamera2_pproc_modules_la_CFLAGS += -DCAMERA_FEATURE_VPE
#endif

libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/includes
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/server-tuning/tuning
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/includes
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/includes
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tools
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/bus
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/controller
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/event
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/module
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/object
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/port
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/stream
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tuning
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/debug
libmmcamera2_pproc_modules_la_CFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common/

libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/includes
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/pproc-new
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/pproc-new/cpp
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/pproc-new/c2d
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/pproc-new/vpe
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/pproc-new/wnr
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/imglib/modules
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/imglib/modules/cac
libmmcamera2_pproc_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/chromatix/0301

libmmcamera2_pproc_modules_la_SOURCES = pproc_module.c
libmmcamera2_pproc_modules_la_SOURCES += pproc_port.c

#LOCAL_SHARED_LIBRARIES := liblog libcutils libmmcamera2_cpp_module liboemcamera libdl libmmcamera2_c2d_module libmmcamera2_imglib_modules libmmcamera2_wnr_module libmmcamera2_vpe_module
libmmcamera2_pproc_modules_la_LDFLAGS = -shared -llog
libmmcamera2_pproc_modules_la_LIBADD = -ldl -llog
libmmcamera2_pproc_modules_la_LIBADD +=  $(BUILD_PATH)/media-controller/mct/liboemcamera.la
libmmcamera2_pproc_modules_la_LIBADD += c2d/libmmcamera2_c2d_module.la
libmmcamera2_pproc_modules_la_LIBADD += wnr/libmmcamera2_wnr_module.la
libmmcamera2_pproc_modules_la_LIBADD += vpe/libmmcamera2_vpe_module.la
libmmcamera2_pproc_modules_la_LIBADD += cpp/libmmcamera2_cpp_module.la

dirs =
dirs += cpp
dirs += c2d
dirs += vpe
dirs += wnr
SUBDIRS = $(dirs)
