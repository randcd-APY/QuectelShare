#======================================================================
#makefile for    -lmmcamera2_isp2_scaler44.so form mm-camera2
#======================================================================
local_ldflags_encoder44 = $(mmcamera_debug_lflags)
AM_CFLAGS =  -DAMSS_VERSION=$(AMSS_VERSION) \
  $(mmcamera_debug_defines) \
  $(mmcamera_debug_cflags)
if !ENABLE_BOARD_QTI_CAMERA_V2
  AM_CFLAGS  += -Werror
endif
mmcamerapath = $(srcdir)/../../../../../../
EXTRA_CFLAGS_encoder44 = -I$(srcdir)
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/includes/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/server-tuning/tuning/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/includes/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/mct/bus/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/mct/controller/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/mct/event/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/mct/module/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/mct/object/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/mct/pipeline/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/mct/port/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/mct/stream/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/mct/tools/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/modules/includes/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/modules/sensors/includes/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/modules/stats/q3a/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/modules/isp2/common/
EXTRA_CFLAGS_encoder44 += -I$(mmcamerapath)/media-controller/modules/isp2/hw/sub_module/
EXTRA_CFLAGS_encoder44 += -I$(CAMERAHAL_PATH)/QCamera2/stack/common
if ENABLE_msm8916
EXTRA_CFLAGS_encoder44 += -I$(srcdir)/include41
endif
if ENABLE_msm8952
EXTRA_CFLAGS_encoder44 += -I$(srcdir)/include41
endif
if ENABLE_msm8937
EXTRA_CFLAGS_encoder44 += -I$(srcdir)/include41
endif
if ENABLE_msm8953
EXTRA_CFLAGS_encoder44 += -I$(srcdir)/include41
endif
if ENABLE_apq8084
EXTRA_CFLAGS_encoder44 += -I$(srcdir)/include44
endif
if ENABLE_msm8992
EXTRA_CFLAGS_encoder44 += -I$(srcdir)/include44
endif
local_src_files_encoder44        = module_scaler44.c port_scaler44.c scaler44.c scaler44_util.c
local_libadd_encoder44 =    -lcutils -lmmcamera2_mct -lmmcamera_isp_sub_module \
    -lmmcamera_dbg -lmmcamera2_isp_modules
if ENABLE_MM_DEBUG
local_libadd_encoder44 +=    -llog
endif

###################################################################################################################
##################################################################################################################

local_ldflags_viewfinder44 = $(mmcamera_debug_lflags)
mmcamerapath = $(srcdir)/../../../../../../
EXTRA_CFLAGS_viewfinder44 = -I$(srcdir)
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/includes/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/server-tuning/tuning/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/includes/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/mct/bus/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/mct/controller/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/mct/event/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/mct/module/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/mct/object/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/mct/pipeline/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/mct/port/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/mct/stream/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/mct/tools/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/modules/includes/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/modules/sensors/includes/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/modules/stats/q3a/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/modules/isp2/common/
EXTRA_CFLAGS_viewfinder44 += -I$(mmcamerapath)/media-controller/modules/isp2/hw/sub_module/
EXTRA_CFLAGS_viewfinder44 += -I$(CAMERAHAL_PATH)/QCamera2/stack/common
if ENABLE_msm8916
EXTRA_CFLAGS_viewfinder44 += -I$(srcdir)/include41
endif
if ENABLE_msm8952
EXTRA_CFLAGS_viewfinder44 += -I$(srcdir)/include41
endif
if ENABLE_msm8937
EXTRA_CFLAGS_viewfinder44 += -I$(srcdir)/include41
endif
if ENABLE_msm8953
EXTRA_CFLAGS_viewfinder44 += -I$(srcdir)/include41
endif
if ENABLE_apq8084
EXTRA_CFLAGS_viewfinder44 += -I$(srcdir)/include44
endif
if ENABLE_msm8992
EXTRA_CFLAGS_viewfinder44 += -I$(srcdir)/include44
endif
local_src_files_viewfinder44        = module_scaler44.c port_scaler44.c scaler44.c scaler44_util.c
local_libadd_viewfinder44 =    -lcutils -lmmcamera_dbg #-lmmcamera2_mct -lmmcamera_isp_sub_module \
    #-lmmcamera_dbg -lmmcamera2_isp_modules
if ENABLE_MM_DEBUG
local_libadd_viewfinder44 +=    -llog
endif

final_sources_viewfinder44=
final_libadd_viewfinder44=
final_sources_encoder44=
final_libadd_encoder44=
if ENABLE_msm8916
final_sources_viewfinder44+=$(local_src_files_viewfinder44)
final_libadd_viewfinder44+=$(local_libadd_viewfinder44)
final_sources_encoder44+=$(local_src_files_encoder44)
final_libadd_encoder44+=$(local_libadd_encoder44)
endif
if ENABLE_msm8952
final_sources_viewfinder44+=$(local_src_files_viewfinder44)
final_libadd_viewfinder44+=$(local_libadd_viewfinder44)
final_sources_encoder44+=$(local_src_files_encoder44)
final_libadd_encoder44+=$(local_libadd_encoder44)
endif
if ENABLE_msm8937
final_sources_viewfinder44+=$(local_src_files_viewfinder44)
final_libadd_viewfinder44+=$(local_libadd_viewfinder44)
final_sources_encoder44+=$(local_src_files_encoder44)
final_libadd_encoder44+=$(local_libadd_encoder44)
endif
if ENABLE_msm8953
final_sources_viewfinder44+=$(local_src_files_viewfinder44)
final_libadd_viewfinder44+=$(local_libadd_viewfinder44)
final_sources_encoder44+=$(local_src_files_encoder44)
final_libadd_encoder44+=$(local_libadd_encoder44)
endif
if ENABLE_apq8084
final_sources_viewfinder44+=$(local_src_files_viewfinder44)
final_libadd_viewfinder44+=$(local_libadd_viewfinder44)
final_sources_encoder44+=$(local_src_files_encoder44)
final_libadd_encoder44+=$(local_libadd_encoder44)
endif
if ENABLE_msm8992
final_sources_viewfinder44+=$(local_src_files_viewfinder44)
final_libadd_viewfinder44+=$(local_libadd_viewfinder44)
final_sources_encoder44+=$(local_src_files_encoder44)
final_libadd_encoder44+=$(local_libadd_encoder44)
endif
libsdir = $(prefix)/lib
libs_LTLIBRARIES  =libmmcamera_isp_scaler_viewfinder44.la 
libs_LTLIBRARIES  +=libmmcamera_isp_scaler_encoder44.la
libmmcamera_isp_scaler_encoder44_la_SOURCES = $(final_sources_encoder44)
libmmcamera_isp_scaler_viewfinder44_la_SOURCES = $(final_sources_viewfinder44)
libmmcamera_isp_scaler_encoder44_la_LIBADD = $(final_libadd_encoder44)
libmmcamera_isp_scaler_viewfinder44_la_LIBADD = $(final_libadd_viewfinder44)
libmmcamera_isp_scaler_encoder44_la_LDFLAGS = -shared ../../../../../mct/libmmcamera2_mct.la ../../../module/libmmcamera2_isp_modules.la ../../sub_module/libmmcamera_isp_sub_module.la
libmmcamera_isp_scaler_viewfinder44_la_LDFLAGS = -shared ../../../../../mct/libmmcamera2_mct.la ../../../module/libmmcamera2_isp_modules.la ../../sub_module/libmmcamera_isp_sub_module.la
libmmcamera_isp_scaler_encoder44_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS_encoder44}
libmmcamera_isp_scaler_viewfinder44_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS_viewfinder44}

