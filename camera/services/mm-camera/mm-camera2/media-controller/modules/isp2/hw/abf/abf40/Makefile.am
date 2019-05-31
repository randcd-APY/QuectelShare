#======================================================================
#makefile for    -lmmcamera_isp_abf40.so for mm-camera2
#======================================================================
EXTRA_CFLAGS =
AM_CFLAGS=
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

if ENABLE_msm8916
EXTRA_CFLAGS += -I$(srcdir)/include40
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp40
endif
if ENABLE_msm8974
EXTRA_CFLAGS += -I$(srcdir)/include40
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp40
endif
if ENABLE_msm8952
EXTRA_CFLAGS += -I$(srcdir)/include40
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp41
endif
if ENABLE_msm8937
EXTRA_CFLAGS += -I$(srcdir)/include40
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp42
endif
if ENABLE_msm8953
EXTRA_CFLAGS += -I$(srcdir)/include40
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp42
endif

local_src_files        = module_abf40.c abf40.c
if ENABLE_OEM_CHROMATIX
local_src_files        += abf40_ext.c
AM_CFLAGS += -DOVERRIDE_FUNC=1
endif
local_libadd =    -lcutils \
   # -L$(mmcamerapath)/media-controller/mct -lmmcamera2_mct \
   # -lmmcamera_isp_sub_module \
    -lmmcamera_dbg
   # -lmmcamera2_isp_modules
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
libsdir = $(prefix)/lib
AM_LDFLAGS = 
libs_LTLIBRARIES  =libmmcamera_isp_abf40.la
libmmcamera_isp_abf40_la_SOURCES = $(final_sources)
libmmcamera_isp_abf40_la_LIBADD = $(final_libadd)
libmmcamera_isp_abf40_la_LDFLAGS = -shared ../../../../../mct/libmmcamera2_mct.la ../../../module/libmmcamera2_isp_modules.la ../../sub_module/libmmcamera_isp_sub_module.la
libmmcamera_isp_abf40_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS}
