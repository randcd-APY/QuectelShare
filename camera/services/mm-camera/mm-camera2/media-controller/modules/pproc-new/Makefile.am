ACLOCAL_AMFLAGS = -I m4
LOCAL_PATH = $(top_srcdir)/media-controller/modules/pproc-new
LOCAL_MMCAMERA_PATH = $(LOCAL_PATH)/../../../../mm-camera2

LOCAL_CPPFLAGS  = -I$(LOCAL_PATH)
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/includes/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/server-tuning/tuning/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/includes/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/bus/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/controller/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/event/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/module/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/object/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/pipeline/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/port/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/stream/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/tools/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/mct/debug/fdleak
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/includes/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/pproc-new/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/pproc-new/cpp/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/pproc-new/c2d/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/modules/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/modules/cac/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/modules/afs/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/imglib/modules/module_imglib/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/pproc-new/base/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/pproc-new/eztune/
LOCAL_CPPFLAGS += -I$(LOCAL_MMCAMERA_PATH)/media-controller/modules/pproc-new/buf_mgr/
LOCAL_CPPFLAGS += -I$(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)
LOCAL_CPPFLAGS += -I$(COMMON_INCDIR)/adreno200/
LOCAL_CPPFLAGS += -I$(COMMON_INCDIR)/adreno/
LOCAL_CPPFLAGS += -I$(COMMON_INCDIR)/mm-camera-interface
LOCAL_CPPFLAGS += -I$(CAMERAHAL_PATH)/QCamera2/stack/common

LOCAL_CFLAGS = -Werror

if FEATURE_CAC
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_CAC
endif

if FEATURE_SVHDR_EN
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_SVHDR
endif


if FEATURE_SW_TNR
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_SW_TNR
endif

LOCAL_CPPFLAGS += -DCAMERA_FEATURE_CPP
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_TMOD
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_LLVD
if !DISABLE_FASTCV
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_LCAC
endif
if !DISABLE_ADRENO_GPU
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_PPEISCORE
if FEATURE_DG
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_PPDGCORE
endif
endif
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_AFS
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_EZT
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_QUADRA_CFA
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_SAT
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_SAC
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_RTBDM
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_RTB

if ENABLE_C2D
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_C2D
endif

if FEATURE_BINCORR
LOCAL_CPPFLAGS += -DCAMERA_FEATURE_BINCORR
endif

LOCAL_SRC_FILES = pproc_module.c
LOCAL_SRC_FILES += pproc_port.c

LOCAL_LDFLAGS = -shared
LOCAL_LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
LOCAL_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap

LOCAL_LIBADD = -llog -lcutils -ldl -lmmcamera_dbg \
		 ../../mct/libmmcamera2_mct.la \
	       	../imglib/libmmcamera2_imglib_modules.la \
		cpp/libmmcamera2_cpp_module.la \
		base/libmmcamera_ppbase_module.la \
	       	eztune/libmmcamera_eztune_module.la

if ENABLE_C2D
LOCAL_LIBADD += c2d/libmmcamera2_c2d_module.la
endif

lib_LTLIBRARIES = libmmcamera2_pproc_modules.la
libmmcamera2_pproc_modules_la_SOURCES = $(LOCAL_SRC_FILES)
libmmcamera2_pproc_modules_la_LIBADD = $(LOCAL_LIBADD)
libmmcamera2_pproc_modules_la_LDFLAGS = $(LOCAL_LDFLAGS)
libmmcamera2_pproc_modules_la_CFLAGS =  $(LOCAL_CFLAGS)
libmmcamera2_pproc_modules_la_CPPFLAGS =  $(LOCAL_CPPFLAGS) 

dirs = 
dirs += buf_mgr
dirs += cpp
if ENABLE_C2D
dirs += c2d
endif
dirs += base
dirs += eztune

SUBDIRS = $(dirs)
