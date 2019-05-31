#======================================================================
#makefile for    -lmmcamera2_isp2_bcc44.so form mm-camera2
#======================================================================
EXTRA_CFLAGS =
AM_CFLAGS=

if !ENABLE_BOARD_QTI_CAMERA_V2
  AM_CFLAGS  += -Werror
endif
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
if ENABLE_msm8974
EXTRA_CFLAGS += -I$(srcdir)/include44
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp44
endif
if ENABLE_apq8084
EXTRA_CFLAGS += -I$(srcdir)/include44
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp44
endif
if ENABLE_msm8992
EXTRA_CFLAGS += -I$(srcdir)/include44
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp44
endif
if ENABLE_msm8994
EXTRA_CFLAGS += -I$(srcdir)/include46
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp46
endif

local_src_files        = module_bcc44.c bcc44.c
local_libadd =    -lcutils \
    # -lmmcamera2_mct \
   # -lmmcamera_isp_sub_module \
    -lmmcamera_dbg
   # -lmmcamera2_isp_modules
if ENABLE_MM_DEBUG
local_libadd +=    -llog
endif

final_sources=
final_libadd=
if ENABLE_apq8084
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
if ENABLE_msm8994
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
if ENABLE_msm8992
final_sources+=$(local_src_files)
final_libadd+=$(local_libadd)
endif
libsdir = $(prefix)/lib
libs_LTLIBRARIES  =libmmcamera_isp_bcc44.la
libmmcamera_isp_bcc44_la_SOURCES = $(final_sources)
libmmcamera_isp_bcc44_la_LIBADD = $(final_libadd)
libmmcamera_isp_bcc44_la_LDFLAGS = -shared ../../../../../mct/libmmcamera2_mct.la ../../../module/libmmcamera2_isp_modules.la ../../sub_module/libmmcamera_isp_sub_module.la
libmmcamera_isp_bcc44_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS}
