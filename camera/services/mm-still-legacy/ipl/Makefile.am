CURRENT_DIR = $(top_srcdir)/ipl

AM_CFLAGS = -g -O3 \
       $(CPU) \
       -Dlrintf=_ffix_r \
       -D__alignx\(x\)=__attribute__\(\(__aligned__\(x\)\)\) \
       -D_POSIX_SOURCE \
       -DPOSIX_C_SOURCE=199506L \
       -D_XOPEN_SOURCE=600 \
       -D_XOPEN_SOURCE_EXTENDED=1 \
       -D_BSD_SOURCE=1 \
       -D_SVID_SOURCE=1 \
       -D_GNU_SOURCE \
       -DT_ARM \
       -D__MSMHW_MODEM_PROC__=1 \
       -D__MSMHW_APPS_PROC__=2 \
       -D__MSMHW_PROC_DEF__=__MSMHW_APPS_PROC__ \
       -DMSMHW_MODEM_PROC -DMSMHW_APPS_PROC \
       -DIMAGE_APPS_PROC \
       -DQC_MODIFIED -Dinline=__inline \
       -DASSERT=ASSERT_FATAL \
       -Dsvcsm_create=svcrtr_create \
       -DCONFIG_MSM7600 \
       -I./system/msm-rpc/libadsp/ -I./system/msm-rpc/librpc/rpc $(KERNEL_HEADERS:%=-I%)

AM_CFLAGS += -fno-short-enums \
       -DIPL_DEBUG_STANDALONE \
       -DFEATURE_QDSP_RTOS \
       -DTRACE_ARM_DSP \
       -DMSM7600 \
       -D_ANDROID_ \
       -D_DEBUG \
       -DTRUE="1" \
       -DFALSE="0"
AM_CFLAGS += -I$(CURRENT_DIR)/src
AM_CFLAGS += -I$(CURRENT_DIR)/inc
AM_CFLAGS += -I$(WORKSPACE)/camera/services/mm-camera-legacy/common
AM_CFLAGS += -include stdint.h
AM_CFLAGS += -include stddef.h

#Make the shared library (libmmipl)
lib_LTLIBRARIES = libmmipl.la

libmmipl_la_CFLAGS = $(AM_CFLAGS)
libmmipl_la_SOURCES = src/ipl_attic.c \
       src/ipl_compose.c \
       src/ipl_convert.c \
       src/ipl_downSize.c \
       src/ipl_efx.c \
       src/ipl_helper.c \
       src/ipl_hjr.c \
       src/ipl_rotAddCrop.c \
       src/ipl_upSize.c \
       src/ipl_util.c \
       src/ipl_xform.c
libmmipl_la_LIBADD = -lcutils
libmmipl_la_LDFLAGS = -shared
