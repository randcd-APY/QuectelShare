#======================================================================
#makefile for    -lmmcamera2_isp2_fovcrop46.so form mm-camera2
#======================================================================
EXTRA_CFLAGS_viewfinder46 =
EXTRA_CFLAGS_encoder46=
EXTRA_CFLAGS_video46 =

local_ldflags_viewfinder46 = $(mmcamera_debug_lflags)
AM_CFLAGS =  -DAMSS_VERSION=$(AMSS_VERSION) \
  $(mmcamera_debug_defines) \
  $(mmcamera_debug_cflags)
AM_CFLAGS  += -Werror -Wno-unused-parameter
mmcamerapath = $(srcdir)/../../../../../../
EXTRA_CFLAGS_viewfinder46 += -I$(srcdir)
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/includes/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/server-tuning/tuning/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/includes/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/mct/bus/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/mct/controller/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/mct/event/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/mct/module/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/mct/object/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/mct/pipeline/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/mct/port/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/mct/stream/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/mct/tools/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/modules/includes/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/modules/sensors/includes/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/modules/isp2/common/
EXTRA_CFLAGS_viewfinder46 += -I$(mmcamerapath)/media-controller/modules/isp2/hw/sub_module
EXTRA_CFLAGS_viewfinder46 += -I$(CAMERAHAL_PATH)/QCamera2/stack/common
if ENABLE_msm8994
EXTRA_CFLAGS_viewfinder46 += -I$(srcdir)/include46
endif
if ENABLE_msm8996
EXTRA_CFLAGS_viewfinder46 += -I$(srcdir)/include47
endif
if ENABLE_msmcobalt
EXTRA_CFLAGS_viewfinder46 += -I$(srcdir)/include47
endif
if ENABLE_sdm660
EXTRA_CFLAGS_viewfinder46 += -I$(srcdir)/include47
endif
if ENABLE_msm8998
EXTRA_CFLAGS_viewfinder46 += -I$(srcdir)/include47
endif
local_src_files_viewfinder46        = module_fovcrop46.c port_fovcrop46.c fovcrop46.c fovcrop46_util.c
local_libadd_viewfinder46 =    -lcutils \
     #libmmcamera2_mct \
    #-lmmcamera_isp_sub_module \
    -lmmcamera_dbg #\
    #-lmmcamera2_isp_modules
if ENABLE_MM_DEBUG
local_libadd_viewfinder46 +=    -llog
endif

###################################################################################################################################
##################################################################################################################################

local_ldflags_encoder46 = $(mmcamera_debug_lflags)
AM_CFLAGS  += -Werror -Wno-unused-parameter
mmcamerapath = $(srcdir)/../../../../../../
EXTRA_CFLAGS_encoder46 += -I$(srcdir)
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/includes/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/server-tuning/tuning/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/includes/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/mct/bus/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/mct/controller/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/mct/event/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/mct/module/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/mct/object/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/mct/pipeline/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/mct/port/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/mct/stream/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/mct/tools/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/modules/includes/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/modules/sensors/includes/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/modules/isp2/common/
EXTRA_CFLAGS_encoder46 += -I$(mmcamerapath)/media-controller/modules/isp2/hw/sub_module
EXTRA_CFLAGS_encoder46 += -I$(CAMERAHAL_PATH)/QCamera2/stack/common
if ENABLE_msm8994
EXTRA_CFLAGS_encoder46 += -I$(srcdir)/include46
endif
if ENABLE_msm8996
EXTRA_CFLAGS_encoder46 += -I$(srcdir)/include47
endif
if ENABLE_msmcobalt
EXTRA_CFLAGS_encoder46 += -I$(srcdir)/include47
endif
if ENABLE_sdm660
EXTRA_CFLAGS_encoder46 += -I$(srcdir)/include47
endif
if ENABLE_msm8998
EXTRA_CFLAGS_encoder46 += -I$(srcdir)/include47
endif
local_src_files_encoder46        = module_fovcrop46.c port_fovcrop46.c fovcrop46.c fovcrop46_util.c
local_libadd_encoder46 =    -lcutils \
    #libmmcamera2_mct \
    #-lmmcamera_isp_sub_module \
    -lmmcamera_dbg #\
    #-lmmcamera2_isp_modules
if ENABLE_MM_DEBUG
local_libadd_encoder46 +=    -llog
endif

##################################################################################################################
##################################################################################################################
local_ldflags_video46 = $(mmcamera_debug_lflags)
AM_CFLAGS  += -Werror -Wno-unused-parameter
mmcamerapath = $(srcdir)/../../../../../../
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/includes/
EXTRA_CFLAGS_video46 += -I$(srcdir)
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/server-tuning/tuning/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/includes/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/mct/bus/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/mct/controller/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/mct/event/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/mct/module/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/mct/object/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/mct/pipeline/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/mct/port/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/mct/stream/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/mct/tools/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/modules/includes/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/modules/sensors/includes/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/modules/isp2/common/
EXTRA_CFLAGS_video46 += -I$(mmcamerapath)/media-controller/modules/isp2/hw/sub_module
EXTRA_CFLAGS_video46 += -I$(CAMERAHAL_PATH)/QCamera2/stack/common
if ENABLE_msm8994
EXTRA_CFLAGS_video46 += -I$(srcdir)/include46
endif
if ENABLE_msm8996
EXTRA_CFLAGS_video46 += -I$(srcdir)/include47
endif
if ENABLE_msmcobalt
EXTRA_CFLAGS_video46 += -I$(srcdir)/include47
endif
if ENABLE_sdm660
EXTRA_CFLAGS_video46 += -I$(srcdir)/include47
endif
if ENABLE_msm8998
EXTRA_CFLAGS_video46 += -I$(srcdir)/include47
endif
local_src_files_video46        = module_fovcrop46.c port_fovcrop46.c fovcrop46.c fovcrop46_util.c
local_libadd_video46 =    -lcutils \
     #libmmcamera2_mct \
    #-lmmcamera_isp_sub_module \
    -lmmcamera_dbg #\
    #-lmmcamera2_isp_modules
if ENABLE_MM_DEBUG
local_libadd_video46 +=    -llog
endif

#############################################################################################################
#############################################################################################################

final_sources_viewfinder46=
final_libadd_viewfinder46=
final_sources_encoder46=
final_libadd_encoder46=
final_sources_video46=
final_libadd_vide046=
if ENABLE_msm8994
final_sources_viewfinder46=$(local_src_files_viewfinder46)
final_libadd_viewfinder46=$(local_libadd_viewfinder46)
final_sources_encoder46=$(local_src_files_encoder46)
final_libadd_encoder46=$(local_libadd_encoder46)
final_sources_video46=$(local_src_files_video46)
final_libadd_vide046=$(local_libadd_video46)
endif
if ENABLE_msm8996
final_sources_viewfinder46=$(local_src_files_viewfinder46)
final_libadd_viewfinder46=$(local_libadd_viewfinder46)
final_sources_encoder46=$(local_src_files_encoder46)
final_libadd_encoder46=$(local_libadd_encoder46)
final_sources_video46=$(local_src_files_video46)
final_libadd_vide046=$(local_libadd_video46)
endif
if ENABLE_msmcobalt
final_sources_viewfinder46=$(local_src_files_viewfinder46)
final_libadd_viewfinder46=$(local_libadd_viewfinder46)
final_sources_encoder46=$(local_src_files_encoder46)
final_libadd_encoder46=$(local_libadd_encoder46)
final_sources_video46=$(local_src_files_video46)
final_libadd_vide046=$(local_libadd_video46)
endif
if ENABLE_sdm660
final_sources_viewfinder46=$(local_src_files_viewfinder46)
final_libadd_viewfinder46=$(local_libadd_viewfinder46)
final_sources_encoder46=$(local_src_files_encoder46)
final_libadd_encoder46=$(local_libadd_encoder46)
final_sources_video46=$(local_src_files_video46)
final_libadd_vide046=$(local_libadd_video46)
endif
if ENABLE_msm8998
final_sources_viewfinder46=$(local_src_files_viewfinder46)
final_libadd_viewfinder46=$(local_libadd_viewfinder46)
final_sources_encoder46=$(local_src_files_encoder46)
final_libadd_encoder46=$(local_libadd_encoder46)
final_sources_video46=$(local_src_files_video46)
final_libadd_vide046=$(local_libadd_video46)
endif
libsdir = $(prefix)/lib
libs_LTLIBRARIES  =libmmcamera_isp_fovcrop_viewfinder46.la 
libs_LTLIBRARIES  +=libmmcamera_isp_fovcrop_encoder46.la
libs_LTLIBRARIES  +=libmmcamera_isp_fovcrop_video46.la
libmmcamera_isp_fovcrop_viewfinder46_la_SOURCES = $(final_sources_viewfinder46) 
libmmcamera_isp_fovcrop_encoder46_la_SOURCES = $(final_sources_encoder46) 
libmmcamera_isp_fovcrop_video46_la_SOURCES = $(final_sources_video46)

libmmcamera_isp_fovcrop_viewfinder46_la_LIBADD = $(final_libadd_viewfinder46) 
libmmcamera_isp_fovcrop_encoder46_la_LIBADD = $(final_libadd_encoder46) 
libmmcamera_isp_fovcrop_video46_la_LIBADD = $(final_libadd_video46)

libmmcamera_isp_fovcrop_viewfinder46_la_LDFLAGS = -shared ../../../../../mct/libmmcamera2_mct.la ../../../module/libmmcamera2_isp_modules.la ../../sub_module/libmmcamera_isp_sub_module.la
libmmcamera_isp_fovcrop_encoder46_la_LDFLAGS = -shared ../../../../../mct/libmmcamera2_mct.la ../../../module/libmmcamera2_isp_modules.la ../../sub_module/libmmcamera_isp_sub_module.la
libmmcamera_isp_fovcrop_video46_la_LDFLAGS = -shared ../../../../../mct/libmmcamera2_mct.la ../../../module/libmmcamera2_isp_modules.la ../../sub_module/libmmcamera_isp_sub_module.la

libmmcamera_isp_fovcrop_viewfinder46_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS_viewfinder46} 
libmmcamera_isp_fovcrop_encoder46_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS_encoder46} 
libmmcamera_isp_fovcrop_video46_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS_video46}
