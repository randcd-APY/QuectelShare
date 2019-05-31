#======================================================================
#makefile for    -lmmcamera2_isp2_bhist_stats44.so form mm-camera2
#======================================================================
EXTRA_CFLAGS =
AM_CFLAGS=

local_ldflags = $(mmcamera_debug_lflags)
AM_CFLAGS =  -DAMSS_VERSION=$(AMSS_VERSION) \
  $(mmcamera_debug_defines) \
  $(mmcamera_debug_cflags)
AM_CFLAGS  += -Werror
mmcamerapath = $(srcdir)/../../../../../../../
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
if ENABLE_msm8916
EXTRA_CFLAGS += -I$(srcdir)/include44
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp40
endif
if ENABLE_msm8974
EXTRA_CFLAGS += -I$(srcdir)/include44
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp40
endif
if ENABLE_msm8952
EXTRA_CFLAGS += -I$(srcdir)/include44
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp41
endif
if ENABLE_msm8994
EXTRA_CFLAGS += -I$(srcdir)/include46
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp46
endif
if ENABLE_msm8996
EXTRA_CFLAGS += -I$(srcdir)/include47
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp47
endif
if ENABLE_msm8937
EXTRA_CFLAGS += -I$(srcdir)/include44
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp42
endif
if ENABLE_msm8953
EXTRA_CFLAGS += -I$(srcdir)/include42
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp42
endif
if ENABLE_msmcobalt
EXTRA_CFLAGS += -I$(srcdir)/include48
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp48
endif
if ENABLE_sdm660
EXTRA_CFLAGS += -I$(srcdir)/include48
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp48
endif
if ENABLE_msm8998
EXTRA_CFLAGS += -I$(srcdir)/include48
EXTRA_CFLAGS += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp48
endif
local_src_files        = module_bhist_stats44.c bhist_stats44.c
if ENABLE_msmcobalt
local_src_files        += include48/bhist_stats_ext48.c
endif
if ENABLE_sdm660
local_src_files        += include48/bhist_stats_ext48.c
endif
if ENABLE_msm8998
local_src_files        += include48/bhist_stats_ext48.c
else
local_src_files        += bhist_stats_ext44.c
endif
local_libadd =    -lcutils -lmmcamera_dbg #\
    #-lmmcamera2_mct \
    #-lmmcamera_isp_sub_module \
    #-lmmcamera_dbg \
    #-lmmcamera2_isp_modules
if ENABLE_MM_DEBUG
local_libadd +=    -llog
endif
#endif # if 8974
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
libs_LTLIBRARIES  =libmmcamera_isp_bhist_stats44.la
libmmcamera_isp_bhist_stats44_la_SOURCES = $(final_sources)
libmmcamera_isp_bhist_stats44_la_LIBADD = $(final_libadd)
libmmcamera_isp_bhist_stats44_la_LDFLAGS = -shared ../../../../../../mct/libmmcamera2_mct.la ../../../../module/libmmcamera2_isp_modules.la ../../../sub_module/libmmcamera_isp_sub_module.la
libmmcamera_isp_bhist_stats44_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS}
#======================================================================
#makefile for    -lmmcamera2_isp2_bhist_stats44.so form mm-camera2
#======================================================================
EXTRA_CFLAGS_hdr =
# Compile HDR BHIST stats    -l
local_ldflags_hdr  = $(mmcamera_debug_lflags)
AM_CFLAGS  += -Werror
mmcamerapath = $(srcdir)/../../../../../../../
EXTRA_CFLAGS_hdr  += -I$(srcdir)
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/includes/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/server-tuning/tuning/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/includes/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/mct/bus/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/mct/controller/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/mct/event/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/mct/module/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/mct/object/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/mct/pipeline/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/mct/port/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/mct/stream/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/mct/tools/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/modules/includes/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/modules/isp2/common/
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/modules/isp2/hw/sub_module
EXTRA_CFLAGS_hdr  += -I$(CAMERAHAL_PATH)/QCamera2/stack/common
if ENABLE_msmcobalt
EXTRA_CFLAGS_hdr  += -I$(srcdir)/hdr_bhist/include48
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp48
endif
if ENABLE_sdm660
EXTRA_CFLAGS_hdr  += -I$(srcdir)/hdr_bhist/include48
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp48
endif
if ENABLE_msm8998
EXTRA_CFLAGS_hdr  += -I$(srcdir)/hdr_bhist/include48
EXTRA_CFLAGS_hdr  += -I$(mmcamerapath)/media-controller/modules/isp2/module/isp48
endif
local_src_files_hdr         = module_bhist_stats44.c bhist_stats44.c

if ENABLE_sdm660
local_src_files_hdr         += hdr_bhist/include48/hdr_bhist_stats_ext48.c
elseif ENABLE_msm8998
local_src_files_hdr         += hdr_bhist/include48/hdr_bhist_stats_ext48.c
else
local_src_files_hdr         += bhist_stats_ext44.c
endif

local_libadd_hdr  =    -lcutils -lmmcamera_dbg #\
    #-lmmcamera2_mct \
    #-lmmcamera_isp_sub_module \
    #-lmmcamera_dbg \
    #-lmmcamera2_isp_modules
if ENABLE_MM_DEBUG
local_libadd_hdr  +=    -llog
endif
final_sources_hdr =
final_libadd_hdr =
if ENABLE_msmcobalt
final_sources_hdr +=$(local_src_files_hdr)
final_libadd_hdr +=$(local_libadd_hdr)
endif
if ENABLE_sdm660
final_sources_hdr +=$(local_src_files_hdr)
final_libadd_hdr +=$(local_libadd_hdr)
endif
if ENABLE_msm8998
final_sources_hdr +=$(local_src_files_hdr)
final_libadd_hdr +=$(local_libadd_hdr)
endif
libsdir = $(prefix)/lib
libs_LTLIBRARIES  +=libmmcamera_isp_hdr_bhist_stats44.la
libmmcamera_isp_hdr_bhist_stats44_la_SOURCES = $(final_sources_hdr)
libmmcamera_isp_hdr_bhist_stats44_la_LIBADD = $(final_libadd_hdr)
libmmcamera_isp_hdr_bhist_stats44_la_LDFLAGS = -shared ../../../../../../mct/libmmcamera2_mct.la ../../../../module/libmmcamera2_isp_modules.la ../../../sub_module/libmmcamera_isp_sub_module.la
libmmcamera_isp_hdr_bhist_stats44_la_CFLAGS = ${AM_CFLAGS} ${EXTRA_CFLAGS_hdr}
