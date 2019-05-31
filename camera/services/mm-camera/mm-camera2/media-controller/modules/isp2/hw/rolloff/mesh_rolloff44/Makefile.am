#======================================================================
#makefile for    -lmmcamera2_isp2_mesh_rolloff44.so form mm-camera2
#======================================================================
EXTRA_CFLAGS =

local_ldflags = $(mmcamera_debug_lflags)
AM_CFLAGS =  -DAMSS_VERSION=$(AMSS_VERSION) \
  $(mmcamera_debug_defines) \
  $(mmcamera_debug_cflags)
AM_CFLAGS  += -Werror
mmcamerapath = $(srcdir)/../../../../../../
EXTRA_CFLAGS += -I$(srcdir)
EXTRA_CFLAGS += -I$(mmcamerapath)/includes/
EXTRA_CFLAGS += -I$(mmcamerapath)/server-tuning/tuning/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/includes/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/mct/bus/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/mct/controller/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/mct/event/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/mct/module/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/mct/object/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/mct/pipeline/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/mct/port/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/mct/stream/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/mct/tools/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/includes/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/common/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/hw/sub_module
EXTRA_CFLAGS += -I$(CAMERAHAL_PATH)/QCamera2/stack/common
if ENABLE_apq8084
EXTRA_CFLAGS += -I$(srcdir)/include44
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp44
endif
if ENABLE_msm8992
EXTRA_CFLAGS += -I$(srcdir)/include44
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp44
endif
if ENABLE_msm8994
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp46
EXTRA_CFLAGS += -I$(srcdir)/include46
endif
if ENABLE_msm8996
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp47
EXTRA_CFLAGS += -I$(srcdir)/include47
EXTRA_CFLAGS += -I$(srcdir)/0309
endif
if ENABLE_msmcobalt
EXTRA_CFLAGS += -I$(srcdir)/include47
EXTRA_CFLAGS += -I$(srcdir)/0310
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp48
endif
if ENABLE_sdm660
EXTRA_CFLAGS += -I$(srcdir)/include47
EXTRA_CFLAGS += -I$(srcdir)/0310
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp48
endif
if ENABLE_msm8998
EXTRA_CFLAGS += -I$(srcdir)/include47
EXTRA_CFLAGS += -I$(srcdir)/0310
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp48
endif
local_src_files        = module_mesh_rolloff44.c \
                          mesh_rolloff44.c
if IS_CHROMATIX_VERSION_0310
    local_src_files += 0310/mesh_rolloff44_0310.c
else
    local_src_files += 0309/mesh_rolloff44_0309.c
endif
if ENABLE_OEM_CHROMATIX
EXTRA_CFLAGS += -I$(LOCAL_EXTEN_ISP_INCLUDES)
local_src_files  += ../../../../../../../../mm-camera-ext/mm-camera2/media-controller/modules/isp2/mesh_rolloff44_ext.c
AM_CFLAGS += -DOVERRIDE_FUNC=1
endif
local_libadd =    -lcutils -lmmcamera_dbg#\
    #-lmmcamera2_mct \
    #-lmmcamera_isp_sub_module \
    #-lmmcamera_dbg \
    #-lmmcamera2_isp_modules
if ENABLE_MM_DEBUG
local_libadd +=    -llog
endif
final_sources=
final_libadd=
if ENABLE_apq8084
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
if ENABLE_msm8992
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
if ENABLE_msm8994
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
if ENABLE_msm8996
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
if ENABLE_msmcobalt
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
if ENABLE_sdm660
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
if ENABLE_msm8998
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
libsdir = $(prefix)/lib
libs_LTLIBRARIES  =libmmcamera_isp_mesh_rolloff44.la
libmmcamera_isp_mesh_rolloff44_la_SOURCES = $(final_sources)
libmmcamera_isp_mesh_rolloff44_la_LIBADD = $(final_libadd)
libmmcamera_isp_mesh_rolloff44_la_LDFLAGS = -shared ../../../../../mct/libmmcamera2_mct.la ../../../module/libmmcamera2_isp_modules.la ../../sub_module/libmmcamera_isp_sub_module.la
libmmcamera_isp_mesh_rolloff44_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS}
