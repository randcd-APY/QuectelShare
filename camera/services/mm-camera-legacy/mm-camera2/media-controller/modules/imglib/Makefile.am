#ACLOCAL_AMFLAGS = -I m4

CURR_PATH = $(top_srcdir)/mm-camera2/media-controller/modules/imglib
MM2_PATH = $(top_srcdir)/mm-camera2
BUILD_PATH = $(top_srcdir)/../../../build/mm-camera2

################ module library ######################
#libmmcamera2_imglib_modules.so
lib_LTLIBRARIES = libmmcamera2_imglib_modules.la libmmcamera_imglib.la libmmcamera_dummyalgo.la

libmmcamera2_imglib_modules_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
libmmcamera2_imglib_modules_la_CFLAGS += -DMSM_CAMERA_GCC
libmmcamera2_imglib_modules_la_CFLAGS += -DHDR_LIB_GHOSTBUSTER
libmmcamera2_imglib_modules_la_CFLAGS += -fPIC
libmmcamera2_imglib_modules_la_CFLAGS += -include stdint.h
libmmcamera2_imglib_modules_la_CFLAGS += -include limits.h
libmmcamera2_imglib_modules_la_CFLAGS += -DLOGE=ALOGE

libmmcamera2_imglib_modules_la_CFLAGS += -include utils/Log.h
libmmcamera2_imglib_modules_la_CFLAGS += -DIDBG_ERROR

libmmcamera2_imglib_modules_la_CFLAGS += -g
libmmcamera2_imglib_modules_la_CFLAGS += -O0
libmmcamera2_imglib_modules_la_CFLAGS += -D_ANDROID_

	ifeq ($(KERNEL_VER), 4.9%)
#libmmcamera2_imglib_modules_la_CFLAGS += -DUSE_4_9_DEFS
	endif

libmmcamera2_imglib_modules_la_CFLAGS += -include img_dbg.h
libmmcamera2_imglib_modules_la_CFLAGS += -DUSE_CAC_V1=false
libmmcamera2_imglib_modules_la_CFLAGS += -Dmmimg_defines=-DLOGE=ALOGE

#if msm8909
libmmcamera2_imglib_modules_la_CFLAGS += -DAF_2X13_FILTER_SUPPORT
libmmcamera2_imglib_modules_la_CFLAGS += -DCAMERA_FEATURE_WNR_SW
#endif

libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/includes

libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/includes
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/includes
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/includes
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tools
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/bus
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/controller
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/event
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/module
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/object
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/port
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/stream
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tuning
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/mct/debug
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/chromatix/0301
libmmcamera2_imglib_modules_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/actuators/0301
libmmcamera2_imglib_modules_la_CFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common/

libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/components/common
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/components/hdr
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/components/wd
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/components/faceproc
#if USE_CAC_V1
#libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/components/cac
#libmmcamera2_imglib_modules_la_CFLAGS += -DUSE_CAC_V1
#else
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/components/cac_v2
#endif

libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/components/include
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/components/lib
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/components/lib/faceproc
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/components/lib/cac
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/modules
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/modules/common
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/modules/faceproc
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/modules/denoise
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/modules/hdr
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/modules/cac
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/modules/base
libmmcamera2_imglib_modules_la_CFLAGS += -I $(CURR_PATH)/utils

libmmcamera2_imglib_modules_la_SOURCES = modules/faceproc/module_faceproc_stab.c
libmmcamera2_imglib_modules_la_SOURCES += modules/faceproc/module_faceproc.c
libmmcamera2_imglib_modules_la_SOURCES += modules/faceproc/module_faceproc_client.c
libmmcamera2_imglib_modules_la_SOURCES += modules/refocus/module_refocus.c
libmmcamera2_imglib_modules_la_SOURCES += modules/llvd/module_llvd.c
libmmcamera2_imglib_modules_la_SOURCES += modules/module_imglib_port.c
libmmcamera2_imglib_modules_la_SOURCES += modules/chroma_flash/module_chroma_flash.c
libmmcamera2_imglib_modules_la_SOURCES += modules/trueportrait/module_trueportrait.c
libmmcamera2_imglib_modules_la_SOURCES += modules/trueportrait/tp_faceproc.c
libmmcamera2_imglib_modules_la_SOURCES += modules/cac/module_cac1_config.c
libmmcamera2_imglib_modules_la_SOURCES += modules/cac/module_cac.c
libmmcamera2_imglib_modules_la_SOURCES += modules/cac/module_cac_client.c
libmmcamera2_imglib_modules_la_SOURCES += modules/cac/module_cac2_config.c
libmmcamera2_imglib_modules_la_SOURCES += modules/module_imglib.c
libmmcamera2_imglib_modules_la_SOURCES += modules/wnr/module_wnr.c
libmmcamera2_imglib_modules_la_SOURCES += modules/wnr/module_wnr_client.c
libmmcamera2_imglib_modules_la_SOURCES += modules/multi_touch_focus/module_multitouchfocus.c
libmmcamera2_imglib_modules_la_SOURCES += modules/dis20/module_dis20.c
libmmcamera2_imglib_modules_la_SOURCES += modules/denoise/module_denoise.c
libmmcamera2_imglib_modules_la_SOURCES += modules/denoise/module_denoise_lib.c
libmmcamera2_imglib_modules_la_SOURCES += modules/denoise/module_denoise_port.c
libmmcamera2_imglib_modules_la_SOURCES += modules/base/module_imgbase.c
libmmcamera2_imglib_modules_la_SOURCES += modules/base/module_imgbase_client.c
libmmcamera2_imglib_modules_la_SOURCES += modules/afs/module_afs_client.c
libmmcamera2_imglib_modules_la_SOURCES += modules/afs/module_afs.c
libmmcamera2_imglib_modules_la_SOURCES += modules/fssr/module_fssr.c
libmmcamera2_imglib_modules_la_SOURCES += modules/optizoom/module_optizoom.c
libmmcamera2_imglib_modules_la_SOURCES += modules/ubifocus/module_ubifocus.c
libmmcamera2_imglib_modules_la_SOURCES += modules/common/module_imglib_common.c
libmmcamera2_imglib_modules_la_SOURCES += modules/hdr/module_hdr_lib.c
libmmcamera2_imglib_modules_la_SOURCES += modules/hdr/module_hdr_port.c
libmmcamera2_imglib_modules_la_SOURCES += modules/hdr/module_hdr.c

libmmcamera2_imglib_modules_la_LDFLAGS = -shared
libmmcamera2_imglib_modules_la_LIBADD = $(BUILD_PATH)/media-controller/mct/liboemcamera.la
libmmcamera2_imglib_modules_la_LIBADD += ./libmmcamera_imglib.la

#libmmcamera_imglib.so
#lib_LTLIBRARIES += libmmcamera_imglib.la

libmmcamera_imglib_la_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION)
libmmcamera_imglib_la_CFLAGS += -DMSM_CAMERA_GCC
libmmcamera_imglib_la_CFLAGS += -DHDR_LIB_GHOSTBUSTER
libmmcamera_imglib_la_CFLAGS += -fPIC
libmmcamera_imglib_la_CFLAGS += -DUSE_ION
libmmcamera_imglib_la_CFLAGS += -DCAMERA_ION_HEAP_ID=ION_CP_MM_HEAP_ID
libmmcamera_imglib_la_CFLAGS += -DLOGE=ALOGE
libmmcamera_imglib_la_CFLAGS += -include utils/Log.h
libmmcamera_imglib_la_CFLAGS += -DUSE_CAC_V1=false

libmmcamera_imglib_la_CFLAGS += -I components
libmmcamera_imglib_la_CFLAGS += -I components/include
libmmcamera_imglib_la_CFLAGS += -I components/common
libmmcamera_imglib_la_CFLAGS += -I components/hdr
libmmcamera_imglib_la_CFLAGS += -I components/wd
libmmcamera_imglib_la_CFLAGS += -I components/faceproc
#if USE_CAC_V1
#libmmcamera_imglib_la_CFLAGS += -I $(CURR_PATH)/components/cac
#else
libmmcamera_imglib_la_CFLAGS += -I $(CURR_PATH)/components/cac_v2
#endif
libmmcamera_imglib_la_CFLAGS += -I $(CURR_PATH)/components/lib
libmmcamera_imglib_la_CFLAGS += -I $(CURR_PATH)/components/lib/faceproc
libmmcamera_imglib_la_CFLAGS += -I $(CURR_PATH)/components/lib/cac
libmmcamera_imglib_la_CFLAGS += -I $(CURR_PATH)/utils
libmmcamera_imglib_la_CFLAGS += -I $(MM2_PATH)
libmmcamera_imglib_la_CFLAGS += -I $(MM2_PATH)/media-controller/modules/sensors/chromatix/0301
libmmcamera_imglib_la_CFLAGS += -I $(CURR_PATH)/components/include
libmmcamera_imglib_la_CFLAGS += -I $(CURR_PATH)/components/common
libmmcamera_imglib_la_CFLAGS += -I $(libmmcamera2_imglib_modules_la_CFLAGS)

libmmcamera_imglib_la_SOURCES = utils/img_queue.c
libmmcamera_imglib_la_SOURCES += utils/img_buffer.c
libmmcamera_imglib_la_SOURCES += components/common/img_common.c
libmmcamera_imglib_la_SOURCES += components/common/img_comp.c
libmmcamera_imglib_la_SOURCES += components/common/img_comp_factory.c
libmmcamera_imglib_la_SOURCES += components/wd/wd_comp.c
libmmcamera_imglib_la_SOURCES += components/hdr/hdr_comp.c
libmmcamera_imglib_la_SOURCES += components/faceproc/faceproc_comp_eng.c
libmmcamera_imglib_la_SOURCES += components/faceproc/faceproc_comp.c
libmmcamera_imglib_la_SOURCES += components/frameproc/frameproc_comp.c
#if USE_CAC_V1
#libmmcamera_imglib_la_SOURCES += components/cac/cac_comp.c
#else
libmmcamera_imglib_la_SOURCES += components/cac_v2/cac_v2_comp.c
#endif

libmmcamera_imglib_la_LDFLAGS = -shared
libmmcamera_imglib_la_LIBADD = -ldl  -lpthread -lrt -lm -llog -lcutils


#libmmcamera_dummyalgo.so
libmmcamera_dummyalgo_la_CFLAGS = -I $(libmmcamera2_imglib_modules_la_CFLAGS)
libmmcamera_dummyalgo_la_SOURCES = test/imgalgo_dummy.c
libmmcamera_dummyalgo_la_LDFLAGS = -shared
libmmcamera_dummyalgo_la_LIBADD = -ldl -lpthread -lrt -lm -llog -lcutils #-lmmcamera_imglib
libmmcamera_dummyalgo_la_LIBADD += libmmcamera_imglib.la

bin_PROGRAMS = mm-imglib-test mm-module-hdr-test mm-module-imglib-test
#mm-imglib-test
mm_imglib_test_CFLAGS = -I $(libmmcamera2_imglib_modules_la_CFLAGS)
mm_imglib_test_SOURCES = components/test/img_test.c
mm_imglib_test_SOURCES += components/test/hdr_test.c
mm_imglib_test_SOURCES += components/test/denoise_test.c
mm_imglib_test_SOURCES += components/test/faceproc_test.c
mm_imglib_test_SOURCES += components/test/cac_test.c
mm_imglib_test_LDFLAGS = -shared
mm_imglib_test_LDADD = -llog -lcutils -ldl
mm_imglib_test_LDADD += libmmcamera_imglib.la

#mm-module-hdr-test
mm_module_hdr_test_CFLAGS = -I $(libmmcamera2_imglib_modules_la_CFLAGS)
mm_module_hdr_test_SOURCES = test/test_module_hdr.c
mm_module_hdr_test_LDFLAGS = -shared
mm_module_hdr_test_LDADD = -llog -lcutils -ldl #-loemcamera
mm_module_hdr_test_LDADD += libmmcamera2_imglib_modules.la
mm_module_hdr_test_LDADD += $(BUILD_PATH)/media-controller/mct/liboemcamera.la

#mm-module-imglib-test
mm_module_imglib_test_CFLAGS = -I $(libmmcamera2_imglib_modules_la_CFLAGS)
mm_module_imglib_test_SOURCES = test/test_module_imglib.c
mm_module_imglib_test_LDFLAGS = -shared
mm_module_imglib_test_LDADD = -llog -lcutils -ldl #-loemcamera
mm_module_imglib_test_LDADD += libmmcamera2_imglib_modules.la
mm_module_imglib_test_LDADD += $(BUILD_PATH)/media-controller/mct/liboemcamera.la
