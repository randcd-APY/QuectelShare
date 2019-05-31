#======================================================================
#makefile for    -lmmcamera2_isp2_sub_module.so form mm-camera2
#======================================================================
EXTRA_CFLAGS =
AM_CFLAGS=-D_ANDROID_
local_ldflags = $(mmcamera_debug_lflags)
AM_CFLAGS +=  -DAMSS_VERSION=$(AMSS_VERSION) \
  $(mmcamera_debug_defines) \
  $(mmcamera_debug_cflags)
if !ENABLE_BOARD_QTI_CAMERA_V2
  AM_CFLAGS  += -Werror
endif
mmcamerapath = $(srcdir)/../../../../../
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
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/stats/q3a/
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/common/
EXTRA_CFLAGS += -I$(CAMERAHAL_PATH)/QCamera2/stack/common
if ENABLE_OEM_CHROMATIX
AM_CFLAGS += -DOVERRIDE_FUNC=1
EXTRA_CFLAGS += -I$(LOCAL_EXTEN_ISP_INCLUDES)
endif
local_src_files        = isp_sub_module.c \
                          isp_sub_module_port.c \
                          isp_sub_module_util.c \
                          isp_sub_module_zoom.c
local_libadd =    -lcutils -lmmcamera_dbg
if ENABLE_MM_DEBUG
local_libadd +=    -llog
endif
final_sources=
final_libadd=
if ENABLE_msm8916
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
if ENABLE_msm8974
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
if ENABLE_msm8952
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
if ENABLE_msm8937
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
if ENABLE_msm8953
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
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
libs_LTLIBRARIES  =libmmcamera_isp_sub_module.la
libmmcamera_isp_sub_module_la_SOURCES = $(final_sources)
libmmcamera_isp_sub_module_la_LIBADD = $(final_libadd)
libmmcamera_isp_sub_module_la_LDFLAGS = -shared ../../../../mct/libmmcamera2_mct.la ../../module/libmmcamera2_isp_modules.la
libmmcamera_isp_sub_module_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS}
