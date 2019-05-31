LOCAL_PATH = $(top_srcdir)/hvx

################# Add constant Start ##############################
lib_LTLIBRARIES = libmmcamera_hvx_add_constant.la

libmmcamera_hvx_add_constant_la_CFLAGS    = -Werror
libmmcamera_hvx_add_constant_la_CPPFLAGS  = -I $(LOCAL_PATH)/
libmmcamera_hvx_add_constant_la_CPPFLAGS += -I $(LOCAL_PATH)/../sensor/includes/$(CHROMATIX_VERSION)/
libmmcamera_hvx_add_constant_la_SOURCES   = lib/add_constant/hvx_add_constant.c
libmmcamera_hvx_add_constant_la_LDFLAGS   = -shared
libmmcamera_hvx_add_constant_la_LDFLAGS  += -ldl -lcutils -llog

################# grid sum Start ##############################
lib_LTLIBRARIES += libmmcamera_hvx_grid_sum.la

libmmcamera_hvx_grid_sum_la_CFLAGS    = -Werror
libmmcamera_hvx_grid_sum_la_CPPFLAGS  = -I $(LOCAL_PATH)/
libmmcamera_hvx_grid_sum_la_CPPFLAGS += -I $(LOCAL_PATH)/../sensor/includes/$(CHROMATIX_VERSION)/
libmmcamera_hvx_grid_sum_la_SOURCES   = lib/grid_sum/hvx_grid_sum.c
libmmcamera_hvx_grid_sum_la_LDFLAGS   = -shared
libmmcamera_hvx_grid_sum_la_LDFLAGS  += -ldl -lcutils -llog

################# zzHDR Start ##############################
lib_LTLIBRARIES += libmmcamera_hvx_zzHDR.la

libmmcamera_hvx_zzHDR_la_CFLAGS    = -Werror
libmmcamera_hvx_zzHDR_la_CPPFLAGS  = -I $(LOCAL_PATH)/
libmmcamera_hvx_zzHDR_la_CPPFLAGS += -I $(LOCAL_PATH)/../sensor/includes/$(CHROMATIX_VERSION)/
libmmcamera_hvx_zzHDR_la_CPPFLAGS += -I $(LOCAL_PATH)/lib/zzHDR
libmmcamera_hvx_zzHDR_la_SOURCES   = lib/zzHDR/hvx_zzHDR.c
libmmcamera_hvx_zzHDR_la_LDFLAGS   = -shared
libmmcamera_hvx_zzHDR_la_LDFLAGS  += -ldl -lcutils -llog

################# Copy Header ##############################
h_sources = $(LOCAL_PATH)/hvx_lib.h
pkgincludedir =  $(includedir)/mm-camera-interface
library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)
