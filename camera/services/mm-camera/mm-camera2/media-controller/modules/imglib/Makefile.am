ACLOCAL_AMFLAGS = -I m4

LOCAL_PATH = $(top_srcdir)/media-controller/modules/imglib
LOCAL_IMGLIB_PATH  = $(LOCAL_PATH)
LOCAL_MMCAMERA_PATH = $(LOCAL_PATH)/../../../../mm-camera2

lib_LTLIBRARIES = libmmcamera2_imglib_modules.la

libmmcamera2_imglib_modules_la_CPPFLAGS = -DHDR_LIB_GHOSTBUSTER

if IS_CHROMATIX_VERSION_0309
libmmcamera2_imglib_modules_la_CPPFLAGS += -DCHROMATIX_309
endif

if IS_CHROMATIX_VERSION_0310
libmmcamera2_imglib_modules_la_CPPFLAGS += -DCHROMATIX_310
endif


libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_PATH)
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/includes/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/includes/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/bus/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/controller/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/event/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/module/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/object/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/pipeline/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/port/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/stream/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/tools/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/fdleak
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/includes/
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/common
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/hdr
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/wd
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/faceproc
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/frameproc
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/cac
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/cac_v2
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/cac_v3
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/include
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/lib
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/lib/faceproc
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/lib/cac
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/modules
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/modules/common
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/modules/base
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/utils
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(COMMON_INCDIR)/mm-camera-interface
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/sensors/chromatix/$(CHROMATIX_VERSION)
libmmcamera2_imglib_modules_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION) 

libmmcamera2_imglib_modules_la_CFLAGS = -Werror -Wno-unused-parameter -Wno-unused-function
libmmcamera2_imglib_modules_la_CFLAGS += -Wno-unused-variable -Wall

if FACEPROC_USE_NEW_WRAPPER
if FD_USE_V65
libmmcamera2_imglib_modules_la_CPPFLAGS  += -DFD_USE_V65
endif
if FD_ENABLE_SW_INTERMITTENT
libmmcamera2_imglib_modules_la_CPPFLAGS  += -DFD_ENABLE_SW_INTERMITTENT
endif
endif

if PROFILE_FD
libmmcamera2_imglib_modules_la_CPPFLAGS  += -DPROFILE_FD
endif

if USE_CAC_V1
libmmcamera2_imglib_modules_la_CPPFLAGS += -DUSE_CAC_V1
endif

if USE_CAC_V2
libmmcamera2_imglib_modules_la_CPPFLAGS += -DUSE_CAC_V2
endif

if USE_CAC_V3
libmmcamera2_imglib_modules_la_CPPFLAGS += -DUSE_CAC_V3
endif

if FD_USES_DSP
libmmcamera2_imglib_modules_la_CPPFLAGS += -DFD_WITH_DSP_NEW
endif

if IMGLIB_USES_HVX
libmmcamera2_imglib_modules_la_CPPFLAGS += -DHVX_ENABLED
endif

if ENABLE_MM_DEBUG
libmmcamera2_imglib_modules_la_CPPFLAGS += -DLOG_TAG=\"CameraService\"
endif

libmmcamera2_imglib_modules_la_SOURCES = \
        modules/binning_correction/module_bincorr.c \
        modules/trueportrait/module_trueportrait.c \
        modules/module_imglib.c \
        modules/chroma_flash/module_chroma_flash.c \
        modules/denoise/module_denoise.c \
        modules/denoise/module_denoise_port.c \
        modules/denoise/module_denoise_lib.c \
        modules/qdc_common/module_qdc_common.c \
        modules/dis20/module_dis20.c \
        modules/llvd/module_llvd.c \
        modules/base/module_imgbase_client.c \
        modules/base/module_imgbase.c \
        modules/common/module_imglib_common.c \
        modules/hdr/module_hdr.c \
        modules/hdr/module_hdr_lib.c \
        modules/hdr/module_hdr_port.c \
        modules/edge_smooth/module_edge_config.c \
        modules/edge_smooth/module_edge_smooth.c \
        modules/afs/module_afs_client.c \
        modules/afs/module_afs.c \
        modules/optizoom/module_optizoom.c \
        modules/module_imglib_port.c \
        modules/cac/module_cac.c \
        modules/cac/module_cac_config.c \
        modules/cac/module_cac_client.c \
        modules/refocus/module_refocus.c \
        modules/mgr/scene_detect_mgr.c \
        modules/stillmore/module_stillmore.c \
        modules/faceproc/module_faceproc_client.c \
        modules/faceproc/module_faceproc_stab.c \
        modules/faceproc/module_faceproc.c \
        modules/sat/module_sat.c \
        modules/quadracfa/module_quadracfa.c \
        modules/wnr/module_wnr_client.c \
        modules/wnr/module_wnr.c \
        modules/paaf/module_paaf.c \
        modules/dcrf/module_dcrf.c \
        modules/svhdr/module_svhdr.c \
        modules/sample/module_sample.c \
        modules/ubifocus/module_ubifocus.c
if !DISABLE_ADRENO_GPU
libmmcamera2_imglib_modules_la_SOURCES += modules/ppeiscore/module_ppeiscore.c
if FEATURE_DG
libmmcamera2_imglib_modules_la_SOURCES += modules/ppdgcore/module_ppdgcore.c
endif
endif
if !DISABLE_FASTCV
libmmcamera2_imglib_modules_la_SOURCES += modules/lcac/module_lcac.c
endif
if ENABLE_C2D
libmmcamera2_imglib_modules_la_SOURCES += modules/sw2d/module_sw2d.c
endif

libmmcamera2_imglib_modules_la_LIBADD = -llog -lcutils ../../mct/libmmcamera2_mct.la libmmcamera_imglib.la -lmmcamera_dbg -ldl

libmmcamera2_imglib_modules_la_LDFLAGS = -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
libmmcamera2_imglib_modules_la_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap
libmmcamera2_imglib_modules_la_LDFLAGS += -shared


lib_LTLIBRARIES += libmmcamera_imglib.la

libmmcamera_imglib_la_CFLAGS = -g -O0 
libmmcamera_imglib_la_CFLAGS += -Werror -Wno-unused-parameter -Wno-unused-function
libmmcamera_imglib_la_CFLAGS += -Wno-unused-variable -Wall

libmmcamera_imglib_la_CPPFLAGS = -include img_dbg.h
libmmcamera_imglib_la_CPPFLAGS += -DLOGE=ALOGE

if TARGET_USES_ION
libmmcamera_imglib_la_CPPFLAGS += -DUSE_ION
endif

libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/common
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/hdr
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/wd
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/faceproc
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/faceproc_dsp
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/faceproc_hw
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/frameproc
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/include
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/cac
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/cac_v2
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/cac_v3
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/lib
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/lib/faceproc
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/lib/faceproc/libincludes/track
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/lib/faceproc/wrappers
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/lib/faceproc/common
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/lib/cac
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/utils
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/includes/
libmmcamera_imglib_la_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/sensors/chromatix/$(CHROMATIX_VERSION)
libmmcamera_imglib_la_CPPFLAGS += -I $(COMMON_INCDIR)/adreno
libmmcamera_imglib_la_CPPFLAGS += -I $(COMMON_INCDIR)/adreno200
libmmcamera_imglib_la_CPPFLAGS += -I $(COMMON_INCDIR)/mm-camera-interface
libmmcamera_imglib_la_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common

if JPEG_DMA
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/jpegdma
endif

if ENABLE_C2D
libmmcamera_imglib_la_CPPFLAGS += -I $(LOCAL_PATH)/components/lib2d
endif

libmmcamera_imglib_la_CPPFLAGS += -include linux/ion.h
libmmcamera_imglib_la_CPPFLAGS += -include linux/videodev2.h

if FD_USES_DSP
libmmcamera_imglib_la_CPPFLAGS += -DFD_WITH_DSP_NEW
endif

libmmcamera_imglib_la_CPPFLAGS += -DHDR_LIB_GHOSTBUSTER
libmmcamera_imglib_la_CPPFLAGS += -D__ANDROID__

if FACEPROC_USE_NEW_WRAPPER
libmmcamera_imglib_la_CPPFLAGS  += -DFACEPROC_USE_NEW_WRAPPER
if FD_USE_V65
libmmcamera_imglib_la_CPPFLAGS  += -DFD_USE_V65
libmmcamera_imglib_la_CPPFLAGS  += -DFD_VERSION=6.5
else
libmmcamera_imglib_la_CPPFLAGS  += -DFD_VERSION=6.0
endif
else
libmmcamera_imglib_la_CPPFLAGS  += -DFD_VERSION=5
endif

if FDDSP_USE_SW_WRAPPER_FOR_ALL
libmmcamera_imglib_la_CPPFLAGS += -DFDDSP_USE_SW_WRAPPER_FOR_ALL
endif

if PROFILE_FD
libmmcamera_imglib_la_CPPFLAGS += -DPROFILE_FD
endif

if ENABLE_msm8953
libmmcamera_imglib_la_CPPFLAGS += -DUSE_SMMU_BUFFERS_FOR_FDDSP
endif

if IMGLIB_USES_FASTCV
libmmcamera_imglib_la_CPPFLAGS += -D__CAM_FCV__
endif

if IS_CHROMATIX_VERSION_0309
libmmcamera_imglib_la_CPPFLAGS += -DCHROMATIX_309
endif

if IS_CHROMATIX_VERSION_0310
libmmcamera_imglib_la_CPPFLAGS += -DCHROMATIX_310
endif

if JPEG_DMA
libmmcamera_imglib_la_CPPFLAGS += -DJPEG_DMA_AVIL
endif

if ENABLE_C2D
libmmcamera_imglib_la_CPPFLAGS += -DENABLE_C2D
endif

libmmcamera_imglib_la_SOURCES = \
		utils/img_queue.c \
        utils/img_buffer.c \
        utils/img_list.c \
        utils/img_thread.c \
        utils/img_rb.c \
        utils/img_dsp_dl_mgr.c \
        utils/img_fcv.c \
        components/common/img_thread_mgr.c \
        components/common/img_common.c \
        components/common/img_meta.c \
        components/common/img_comp.c \
        components/common/img_comp_factory.c \
        components/wd/wd_comp.c \
        components/hdr/hdr_comp.c \
        components/faceproc/faceproc_comp.c \
        components/faceproc_dsp/faceproc_dsp_comp.c \
        components/faceproc_hw/faceproc_hw_comp.c \
        components/faceproc_hw/faceproc_hw_tracker_wrapper.c \
        components/lib/faceproc/common/faceproc_common.c \
        components/lib/faceproc/wrappers/facial_parts_wrapper.c \
        components/frameproc/frameproc_comp.c \
        components/frameproc/dual_frameproc_comp.c \
        components/multiframeproc/multiframeproc_comp.c \
        components/cac/cac_comp.c \
        components/cac_v2/cac_v2_comp.c \
        components/cac_v3/cac_v3_comp.c

libmmcamera_imglib_la_SOURCES += \
        components/common/ds_2by2_asm.s \
        components/common/boost_armv7_gcc.s

if ENABLE_C2D
libmmcamera_imglib_la_SOURCES += \
    components/lib2d/lib2d_comp.c \
    components/lib2d/c2d_utils.c
endif

if JPEG_DMA
libmmcamera_imglib_la_SOURCES += components/jpegdma/jpegdma_comp.c
libmmcamera_imglib_la_SOURCES += components/jpegdma/jpegdma_util.c
endif

if FACEPROC_USE_NEW_WRAPPER
libmmcamera_imglib_la_SOURCES += components/lib/faceproc/wrappers/faceproc_sw_wrapper.c \
		components/lib/faceproc/wrappers/faceproc_lib_wrapper.c \
		components/faceproc/faceproc_comp_engine.c
else
libmmcamera_imglib_la_SOURCES += components/faceproc/faceproc_comp_eng.c
endif

if FDDSP_USE_SW_WRAPPER_FOR_ALL
libmmcamera_imglib_la_SOURCES += components/faceproc_dsp/faceproc_dsp_comp_engine.c
else
libmmcamera_imglib_la_SOURCES += components/faceproc_dsp/faceproc_dsp_comp_eng.c
endif

libmmcamera_imglib_la_LIBADD= -ldl -lcutils -llog -lmmcamera_dbg

if IMGLIB_USES_FASTCV
libmmcamera_imglib_la_LIBADD += -lfastcvopt
endif

################ Imglib headers copy ############
h_sources = \
	 utils/img_buffer.h \
	 components/include/img_common.h \
	 components/include/img_comp.h \
	 components/include/img_meta.h \
	 components/include/img_comp_factory.h \
	 components/lib/img_mem_ops.h \
	 components/lib/img_thread_ops.h

if ENABLE_C2D
h_sources += \
        components/include/lib2d.h
endif

pkgincludedir = $(includedir)/mm-camera/imglib
library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)

################ Face Detection adsp stub library ############

if FD_USES_DSP

lib_LTLIBRARIES += libmmcamera_imglib_faceproc_adspstub.la

libmmcamera_imglib_faceproc_adspstub_la_CPPFLAGS = -I $(LOCAL_PATH)/components/faceproc_dsp
libmmcamera_imglib_faceproc_adspstub_la_SOURCES = components/faceproc_dsp/adsp_fd_stub.c
libmmcamera_imglib_faceproc_adspstub_la_LIBADD = -lcutils -ldl -llog

if USE_CDSP
libmmcamera_imglib_faceproc_adspstub_la_LIBADD += -lcdsprpc
else
libmmcamera_imglib_faceproc_adspstub_la_LIBADD += -ladsprpc
endif
libmmcamera_imglib_faceproc_adspstub_la_LDFLAGS = -shared

endif

################ Face Detection adsp stub v6.x library ############

if FD_USES_DSP

lib_LTLIBRARIES += libmmcamera_imglib_faceproc_adspstub2.la
libmmcamera_imglib_faceproc_adspstub2_la_CPPFLAGS = -I $(LOCAL_PATH)/components/faceproc_dsp
libmmcamera_imglib_faceproc_adspstub2_la_SOURCES = components/faceproc_dsp/stub/adsp_fd_stub.c
libmmcamera_imglib_faceproc_adspstub2_la_LIBADD = -lcutils -ldl -llog
if USE_CDSP
libmmcamera_imglib_faceproc_adspstub2_la_LIBADD += -lcdsprpc
else
libmmcamera_imglib_faceproc_adspstub2_la_LIBADD += -ladsprpc
endif
libmmcamera_imglib_faceproc_adspstub2_la_LDFLAGS = -shared

endif

################ Face Detection Q6 prebuilt library #############

##########################test app ##############################
bin_PROGRAMS = mm-imglib-test

mm_imglib_test_CFLAGS = -g -O0 
mm_imglib_test_CFLAGS += -Werror -Wno-unused-parameter -Wno-unused-function
mm_imglib_test_CFLAGS += -Wno-unused-variable -Wall

mm_imglib_test_CPPFLAGS = -DLOGE=ALOGE

mm_imglib_test_CPPFLAGS += -include img_dbg.h
mm_imglib_test_CPPFLAGS += -include linux/ion.h
mm_imglib_test_CPPFLAGS += -include linux/videodev2.h

if TARGET_USES_ION
mm_imglib_test_CPPFLAGS += -DUSE_ION
endif

mm_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/components/common
mm_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/components/hdr
mm_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/components/wd
mm_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/components/faceproc
mm_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/components/cac
mm_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/components/frameproc
mm_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/components/include
mm_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/components/include/faceproc
mm_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/components/test
mm_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/components/lib
mm_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/utils
mm_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/includes/
mm_imglib_test_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
mm_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/sensors/chromatix/$(CHROMATIX_VERSION)
mm_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/components/lib
mm_imglib_test_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common
mm_imglib_test_CPPFLAGS += -I $(COMMON_INCDIR)/mm-camera-interface

mm_imglib_test_CPPFLAGS += -DHDR_LIB_GHOSTBUSTER

if IS_CHROMATIX_VERSION_0309
mm_imglib_test_CPPFLAGS += -DCHROMATIX_309
endif

if IS_CHROMATIX_VERSION_0310
mm_imglib_test_CPPFLAGS += -DCHROMATIX_310
endif


mm_imglib_test_SOURCES = \
        components/test/img_test.c \
        components/test/hdr_test.c \
        components/test/denoise_test.c \
        components/test/faceproc_test.c \
        components/test/cac_test.c \
        components/test/dual_frameproc_test.c \
        components/test/buf_pool_test.c

mm_imglib_test_LDADD =  ./libmmcamera_imglib.la -lcutils -ldl -lmmcamera_dbg

##########################hdr module test app ##############################

bin_PROGRAMS += mm-module-hdr-test

mm_module_hdr_test_CFLAGS = -g -O0 
mm_module_hdr_test_CFLAGS += -Werror -Wno-unused-parameter -Wno-unused-function
mm_module_hdr_test_CFLAGS += -Wno-unused-variable -Wall

mm_module_hdr_test_CPPFLAGS = -DLOGE=ALOGE

if IS_CHROMATIX_VERSION_0309
mm_module_hdr_test_CPPFLAGS += -DCHROMATIX_309
endif

if IS_CHROMATIX_VERSION_0310
mm_module_hdr_test_CPPFLAGS += -DCHROMATIX_310
endif

if TARGET_USES_ION
mm_module_hdr_test_CPPFLAGS += -DUSE_ION
endif

mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/includes/
mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/includes/
mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/bus/
mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/controller/
mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/event/
mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/module/
mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/object/
mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/pipeline/
mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/port/
mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/stream/
mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/tools/
mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_PATH)/utils
mm_module_hdr_test_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
mm_module_hdr_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/sensors/chromatix/$(CHROMATIX_VERSION)
mm_module_hdr_test_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common
mm_module_hdr_test_SOURCES = test/test_module_hdr.c

mm_module_hdr_test_LDADD = ./libmmcamera2_imglib_modules.la -lcutils -ldl ../../mct/libmmcamera2_mct.la -lmmcamera_dbg
##########################module test app ##############################
bin_PROGRAMS += mm-module-imglib-test

mm_module_imglib_test_CFLAGS = -g -O0 
mm_module_imglib_test_CFLAGS += -Werror -Wno-unused-parameter -Wno-unused-function
mm_module_imglib_test_CFLAGS += -Wno-unused-variable -Wall

mm_module_imglib_test_CPPFLAGS = -DLOGE=ALOGE

if IS_CHROMATIX_VERSION_0309
mm_module_imglib_test_CPPFLAGS += -DCHROMATIX_309
endif

if IS_CHROMATIX_VERSION_0310
mm_module_imglib_test_CPPFLAGS += -DCHROMATIX_310
endif

if TARGET_USES_ION
mm_module_imglib_test_CPPFLAGS += -DUSE_ION
endif

mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/includes/
mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/includes/
mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/bus/
mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/controller/
mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/event/
mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/module/
mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/object/
mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/pipeline/
mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/port/
mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/stream/
mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/mct/tools/
mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_PATH)/utils
mm_module_imglib_test_CPPFLAGS += -I $(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
mm_module_imglib_test_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/sensors/chromatix/$(CHROMATIX_VERSION)
mm_module_imglib_test_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common

mm_module_imglib_test_SOURCES = test/test_module_imglib.c

mm_module_imglib_test_LDADD = ./libmmcamera2_imglib_modules.la -lcutils -ldl ../../mct/libmmcamera2_mct.la -lmmcamera_dbg

##########################module test app ##############################
lib_LTLIBRARIES += libmmcamera_dummyalgo.la

libmmcamera_dummyalgo_la_CFLAGS = -g -O0 
libmmcamera_dummyalgo_la_CFLAGS += -Werror -Wno-unused-parameter -Wno-unused-function
libmmcamera_dummyalgo_la_CFLAGS += -Wno-unused-variable -Wall

libmmcamera_dummyalgo_la_CPPFLAGS = -include img_dbg.h
libmmcamera_dummyalgo_la_CPPFLAGS += -DLOGE=ALOGE

if IS_CHROMATIX_VERSION_0309
libmmcamera_dummyalgo_la_CPPFLAGS += -DCHROMATIX_309
endif

if IS_CHROMATIX_VERSION_0310
libmmcamera_dummyalgo_la_CPPFLAGS += -DCHROMATIX_310
endif

if TARGET_USES_ION
libmmcamera_dummyalgo_la_CPPFLAGS += -DUSE_ION
endif

libmmcamera_dummyalgo_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/includes/
libmmcamera_dummyalgo_la_CPPFLAGS += -I $(COMMON_INCDIR)/mm-camera-interface
libmmcamera_dummyalgo_la_CPPFLAGS += -I $(LOCAL_PATH)/components/include
libmmcamera_dummyalgo_la_CPPFLAGS += -I $(LOCAL_PATH)/components/lib
libmmcamera_dummyalgo_la_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common


libmmcamera_dummyalgo_la_SOURCES = test/imgalgo_dummy.c

libmmcamera_dummyalgo_la_LIBADD = ./libmmcamera2_imglib_modules.la -lcutils -ldl -lmmcamera_dbg -llog

####################### mm-camera-jpegdma-v4l2-test ###############################

LOCAL_JPEGDMA_TEST_PATH = $(top_srcdir)/media-controller/modules/imglib/components/jpegdma_test

bin_PROGRAMS += mm-camera-jpegdma-v4l2-test

mm_camera_jpegdma_v4l2_test_CPPFLAGS  = -I $(LOCAL_JPEGDMA_TEST_PATH)/../include
mm_camera_jpegdma_v4l2_test_CPPFLAGS += -I $(LOCAL_JPEGDMA_TEST_PATH)/../lib
mm_camera_jpegdma_v4l2_test_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common
mm_camera_jpegdma_v4l2_test_CPPFLAGS += -I $(LOCAL_JPEGDMA_TEST_PATH)/../../utils/
mm_camera_jpegdma_v4l2_test_CPPFLAGS += -I $(LOCAL_JPEGDMA_TEST_PATH)/../../../../../includes

mm_camera_jpegdma_v4l2_test_CFLAGS = -Werror

mm_camera_jpegdma_v4l2_test_SOURCES = components/jpegdma_test/jpegdma_test.c

mm_camera_jpegdma_v4l2_test_LDADD = -ldl -lcutils -llog
mm_camera_jpegdma_v4l2_test_LDADD += ./libmmcamera_imglib.la -lmmcamera_dbg

###################################d remosaic library to be used ###################
lib_LTLIBRARIES += libremosaic_daemon.la

libremosaic_daemon_la_CFLAGS = -Wno-non-virtual-dtor -fno-exceptions

libremosaic_daemon_la_CPPFLAGS = -DUSE_API1_REMOSAIC_LIB_DUMMY
libremosaic_daemon_la_CPPFLAGS += -include stdlib.h

libremosaic_daemon_la_CPPFLAGS += \
               -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/include \
               -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/common \
               -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/test \
               -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/lib \
               -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/utils \
	       -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/quadracfa/prebuilt 

libremosaic_daemon_la_SOURCES = components/quadracfa/prebuilt/dummy/remosaic_dummy.cpp

libremosaic_daemon_la_LIBADD = -ldl -lcutils -llog

###################################################################################

lib_LTLIBRARIES += libmmcamera_quadracfa.la

libmmcamera_quadracfa_la_CFLAGS = -Wno-non-virtual-dtor -fno-exceptions

libmmcamera_quadracfa_la_CPPFLAGS = \
               -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/include \
               -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/common \
               -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/test \
               -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/lib \
               -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/utils \
	       -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/quadracfa/prebuilt 

libmmcamera_quadracfa_la_CPPFLAGS += -include stdlib.h
libmmcamera_quadracfa_la_CPPFLAGS += -include stdint.h

libmmcamera_quadracfa_la_SOURCES = components/quadracfa/QCameraQuadraCfa.cpp

libmmcamera_quadracfa_la_LIBADD = -ldl -lcutils -llog
libmmcamera_quadracfa_la_LIBADD += ./libremosaic_daemon.la

########################################################################################
if !DISABLE_ADRENO_GPU
lib_LTLIBRARIES += libmmcamera_ppeiscore.la

libmmcamera_ppeiscore_la_CFLAGS = -Wno-non-virtual-dtor -fno-exceptions
 
libmmcamera_ppeiscore_la_CPPFLAGS  = -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/include/
libmmcamera_ppeiscore_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/utils/
libmmcamera_ppeiscore_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/lib/
libmmcamera_ppeiscore_la_CPPFLAGS += -I $(COMMON_INCDIR)/diag/include
libmmcamera_ppeiscore_la_CPPFLAGS += -I $(COMMON_INCDIR)/qcom/display
libmmcamera_ppeiscore_la_CPPFLAGS += -I system/core/include
libmmcamera_ppeiscore_la_CPPFLAGS += -I $(COMMON_INCDIR)/adreno
libmmcamera_ppeiscore_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/includes/
libmmcamera_ppeiscore_la_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common
libmmcamera_ppeiscore_la_CPPFLAGS += -include $(LOCAL_IMGLIB_PATH)/components/include/img_dbg.h

libmmcamera_ppeiscore_la_SOURCES = components/lib/ppeiscore/QCameraPPEisCore.cpp \
				  components/lib/ppeiscore/QCameraBufferMap.cpp

libmmcamera_ppeiscore_la_LIBADD = -ldl -lcutils -llog -lEGL_adreno -lGLESv2_adreno
libmmcamera_ppeiscore_la_LIBADD += -lhardware -lqdMetaData -lutils -lui -lmmcamera_dbg ./libmmcamera_imglib.la
endif

########################################################################################
if !DISABLE_ADRENO_GPU
if FEATURE_DG
lib_LTLIBRARIES += libmmcamera_ppdgcore.la

libmmcamera_ppdgcore_la_CFLAGS = -Wno-non-virtual-dtor -fno-exceptions

libmmcamera_ppdgcore_la_CPPFLAGS  = -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/include/
libmmcamera_ppdgcore_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/utils/
libmmcamera_ppdgcore_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/components/lib/
libmmcamera_ppdgcore_la_CPPFLAGS += -I $(COMMON_INCDIR)/diag/include
libmmcamera_ppdgcore_la_CPPFLAGS += -I $(COMMON_INCDIR)/qcom/display
libmmcamera_ppdgcore_la_CPPFLAGS += -I system/core/include
libmmcamera_ppdgcore_la_CPPFLAGS += -I $(COMMON_INCDIR)/adreno
#For DG, which is copied from drones-prebuilts
libmmcamera_ppdgcore_la_CPPFLAGS += -I$(WORKSPACE)/FEAT-API-LNX-DG/apps_proc

libmmcamera_ppdgcore_la_CPPFLAGS += -I $(LOCAL_MMCAMERA_PATH)/includes/
libmmcamera_ppdgcore_la_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common
libmmcamera_ppdgcore_la_CPPFLAGS += -include $(LOCAL_IMGLIB_PATH)/components/include/img_dbg.h

libmmcamera_ppdgcore_la_SOURCES = components/lib/ppdgcore/QCameraPPDgCore.cpp

libmmcamera_ppdgcore_la_LIBADD = -ldl -lcutils -llog
libmmcamera_ppdgcore_la_LIBADD += -lhardware -lqdMetaData -lutils -lui -lmmcamera_dbg ./libmmcamera_imglib.la
endif
endif
#################################################################################################
if ENABLE_C2D
lib_LTLIBRARIES += libmmcamera_sw2d_lib.la

libmmcamera_sw2d_lib_la_CFLAGS = -Wno-non-virtual-dtor -fno-exceptions

libmmcamera_sw2d_lib_la_CPPFLAGS = -I $(LOCAL_IMGLIB_PATH)/utils/
libmmcamera_sw2d_lib_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/include/
libmmcamera_sw2d_lib_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/lib/
libmmcamera_sw2d_lib_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/components/sw2d/
libmmcamera_sw2d_lib_la_CPPFLAGS += -I $(LOCAL_IMGLIB_PATH)/../../../includes
libmmcamera_sw2d_lib_la_CPPFLAGS += -I $(COMMON_INCDIR)/mm-camera-interface
libmmcamera_sw2d_lib_la_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common

if USE_FASTCV_OPT
libmmcamera_sw2d_lib_la_CPPFLAGS += -DUSE_FASTCV_OPT
endif

libmmcamera_sw2d_lib_la_SOURCES = components/lib/sw2d/QCameraPostProcSW2D.cpp

libmmcamera_sw2d_lib_la_LIBADD = ./libmmcamera_imglib.la -lmmcamera_dbg
libmmcamera_sw2d_lib_la_LIBADD += -ldl -lcutils -llog

if USE_FASTCV_OPT
libmmcamera_sw2d_lib_la_LIBADD += -lfastcvopt
endif
endif
dirs = 
SUBDIRS = $(dirs)
