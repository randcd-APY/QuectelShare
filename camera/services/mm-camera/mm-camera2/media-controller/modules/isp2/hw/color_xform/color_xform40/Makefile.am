#======================================================================
#makefile for    -lmmcamera2_isp2_color_xform40.so form mm-camera2
#======================================================================
EXTRA_CFLAGS_viewfinder40 =
EXTRA_CFLAGS_encoder40 =
local_ldflags_viewfinder40  = $(mmcamera_debug_lflags)
AM_CFLAGS =  -DAMSS_VERSION=$(AMSS_VERSION) \
  $(mmcamera_debug_defines) \
  $(mmcamera_debug_cflags)
if !ENABLE_BOARD_QTI_CAMERA_V2
  AM_CFLAGS  += -Werror
endif
mmcamerapath = $(srcdir)/../../../../../../
EXTRA_CFLAGS_viewfinder40 += -I$(srcdir)
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/includes/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/server-tuning/tuning/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/includes/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/mct/bus/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/mct/controller/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/mct/event/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/mct/module/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/mct/object/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/mct/pipeline/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/mct/port/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/mct/stream/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/mct/tools/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/modules/includes/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/modules/isp2/common/
EXTRA_CFLAGS_viewfinder40 += -I$(mmcamerapath)/media-controller/modules/isp2/hw/sub_module
EXTRA_CFLAGS_viewfinder40 += -I$(CAMERAHAL_PATH)/QCamera2/stack/common
local_src_files_viewfinder40        = module_color_xform40.c color_xform40.c
local_libadd_viewfinder40 =    -lcutils \
    #-lmmcamera2_mct \
    #-lmmcamera_isp_sub_module \
    -lmmcamera_dbg# \
    #-lmmcamera2_isp_modules
if ENABLE_MM_DEBUG
local_libadd_viewfinder40 +=    -llog
endif


local_ldflags_encoder40 = $(mmcamera_debug_lflags)
if !ENABLE_BOARD_QTI_CAMERA_V2
  AM_CFLAGS  += -Werror
endif
mmcamerapath = $(srcdir)/../../../../../../
EXTRA_CFLAGS_encoder40 += -I$(srcdir)
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/includes/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/server-tuning/tuning/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/includes/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/mct/bus/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/mct/controller/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/mct/event/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/mct/module/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/mct/object/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/mct/pipeline/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/mct/port/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/mct/stream/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/mct/tools/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/modules/includes/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/modules/isp2/common/
EXTRA_CFLAGS_encoder40 += -I$(mmcamerapath)/media-controller/modules/isp2/hw/sub_module
EXTRA_CFLAGS_encoder40 += -I$(CAMERAHAL_PATH)/QCamera2/stack/common
local_src_files_encoder40        = module_color_xform40.c color_xform40.c
local_libadd_encoder40 =    -lcutils \
    -lmmcamera2_mct \
    -lmmcamera_isp_sub_module \
    -lmmcamera_dbg \
    -lmmcamera2_isp_modules
if ENABLE_MM_DEBUG
local_libadd_encoder40 +=    -llog
endif


final_sources_viewfinder40 =
final_libadd_viewfinder40 =
final_sources_encoder40 =
final_libadd_encoder40 =
if ENABLE_msm8916
final_sources_viewfinder40+=$(local_src_files_viewfinder40)
final_libadd_viewfinder40+=$(local_libadd_viewfinder40)
final_sources_encoder40+=$(local_src_files_encoder40)
final_libadd_encoder40+=$(local_libadd_encoder40)
endif
if ENABLE_msm8974
final_sources_viewfinder40+=$(local_src_files_viewfinder40)
final_libadd_viewfinder40+=$(local_libadd_viewfinder40)
final_sources_encoder40+=$(local_src_files_encoder40)
final_libadd_encoder40+=$(local_libadd_encoder40)
endif
if ENABLE_msm8952
final_sources_viewfinder40+=$(local_src_files_viewfinder40)
final_libadd_viewfinder40+=$(local_libadd_viewfinder40)
final_sources_encoder40+=$(local_src_files_encoder40)
final_libadd_encoder40+=$(local_libadd_encoder40)
endif
if ENABLE_msm8937
final_sources_viewfinder40+=$(local_src_files_viewfinder40)
final_libadd_viewfinder40+=$(local_libadd_viewfinder40)
final_sources_encoder40+=$(local_src_files_encoder40)
final_libadd_encoder40+=$(local_libadd_encoder40)
endif
if ENABLE_msm8953
final_sources_viewfinder40+=$(local_src_files_viewfinder40)
final_libadd_viewfinder40+=$(local_libadd_viewfinder40)
final_sources_encoder40+=$(local_src_files_encoder40)
final_libadd_encoder40+=$(local_libadd_encoder40)
endif
if ENABLE_apq8084
final_sources_viewfinder40+=$(local_src_files_viewfinder40)
final_libadd_viewfinder40+=$(local_libadd_viewfinder40)
final_sources_encoder40+=$(local_src_files_encoder40)
final_libadd_encoder40+=$(local_libadd_encoder40)
endif
if ENABLE_msm8992
final_sources_viewfinder40+=$(local_src_files_viewfinder40)
final_libadd_viewfinder40+=$(local_libadd_viewfinder40)
final_sources_encoder40+=$(local_src_files_encoder40)
final_libadd_encoder40+=$(local_libadd_encoder40)
endif
libsdir = $(prefix)/lib
libs_LTLIBRARIES   =libmmcamera_isp_color_xform_viewfinder40.la
libs_LTLIBRARIES  +=libmmcamera_isp_color_xform_encoder40.la
libmmcamera_isp_color_xform_viewfinder40_la_SOURCES = $(final_sources_viewfinder40)
libmmcamera_isp_color_xform_encoder40_la_SOURCES = $(final_sources_encoder40)
libmmcamera_isp_color_xform_viewfinder40_la_LIBADD = $(final_libadd_viewfinder40) 
libmmcamera_isp_color_xform_encoder40_la_LIBADD = $(final_libadd_encoder40)
libmmcamera_isp_color_xform_viewfinder40_la_LDFLAGS = -shared ../../../../../mct/libmmcamera2_mct.la ../../../module/libmmcamera2_isp_modules.la ../../sub_module/libmmcamera_isp_sub_module.la
libmmcamera_isp_color_xform_encoder40_la_LDFLAGS = -shared ../../../../../mct/libmmcamera2_mct.la ../../../module/libmmcamera2_isp_modules.la ../../sub_module/libmmcamera_isp_sub_module.la
libmmcamera_isp_color_xform_viewfinder40_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS_viewfinder40}
libmmcamera_isp_color_xform_encoder40_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS_encoder40}
