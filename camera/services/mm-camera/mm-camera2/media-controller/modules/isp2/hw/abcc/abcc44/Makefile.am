libsdir = $(prefix)/lib

AM_CFLAGS = -D_ANDROID_

AM_CPPFLAGS = -I../../include

mmcamerapath = $(srcdir)/../../../../../../

AM_CFLAGS += -I$(mmcamerapath)/includes/       \
             -I$(srcdir)/include44/       \
             -I$(srcdir)/../abcc_algo     \
             -I$(mmcamerapath)/server-tuning/tuning/       \
             -I$(mmcamerapath)/media-controller/includes/       \
             -I$(mmcamerapath)/media-controller/includes/       \
             -I$(mmcamerapath)/media-controller/mct/includes/       \
             -I$(mmcamerapath)/media-controller/mct/tools/       \
             -I$(mmcamerapath)/media-controller/mct/bus/       \
             -I$(mmcamerapath)/media-controller/mct/controller/       \
             -I$(mmcamerapath)/media-controller/mct/event/       \
             -I$(mmcamerapath)/media-controller/mct/module/       \
             -I$(mmcamerapath)/media-controller/mct/object/       \
             -I$(mmcamerapath)/media-controller/mct/pipeline/       \
             -I$(mmcamerapath)/media-controller/mct/port/       \
             -I$(mmcamerapath)/media-controller/mct/stream/     \
             -I$(mmcamerapath)/media-controller/modules/includes/      \
             -I$(mmcamerapath)/media-controller/modules/isp2/common/      \
             -I$(mmcamerapath)/media-controller/modules/isp2/hw/sub_module   \
             -I$(src_CAMERAHAL_PATH)/QCamera2/stack/common      

local_sources = module_abcc44.c       \
                abcc44.c      \
                ../abcc_algo/abcc_algo.c

local_libadd = -lcutils \
               -lmmcamera_isp_sub_module \
               -lmmcamera_dbg \
               -lmmcamera2_isp_modulesi

if ENABLE_msm8084
#local_libadd += -loemcamera
 AM_CFLAGS += -I$(srcdir)/include44    \
              -I$(mmcamerapath)/media-controller/modules/isp2/module/isp44

else 
if ENABLE_msm8992
 local_libadd  += -loemcamera
 AM_CFLAGS += -I$(srcdir)/include44    \
             -I$(mmcamerapath)/media-controller/modules/isp2/module/isp44

else
if ENABLE_msm8952
  local_libadd += -lmmcamera2_mct
  AM_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp41

else
if ENABLE_msm8976
  local_libadd += -lmmcamera2_mct
  AM_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp41
else
  local_sources = 
  local_libadd  = 
endif
endif
endif
endif

local_sources_final=
local_libadd_final=
if ENABLE_msm8992
local_sources_final += $(local_sources)
local_libadd_final += $(local_libadd)
endif
if ENABLE_msm8084
local_sources_final+= $(local_sources)
local_libadd_final+= $(local_libadd)
endif
if ENABLE_msm8952
local_sources_final+= $(local_sources)
local_libadd_final+= $(local_libadd)
endif
if ENABLE_msm8976
local_sources_final+= $(local_sources)
local_libadd_final+= $(local_libadd)
endif
libs_LTLIBRARIES  = libmmcamera_isp_abcc44.la
libmmcamera_isp_abcc44_la_SOURCES = $(local_sources_final)
libmmcamera_isp_abcc44_la_LIBADD = $(local_libadd_final)
libmmcamera_isp_abcc44_la_LDFLAGS = -shared 
