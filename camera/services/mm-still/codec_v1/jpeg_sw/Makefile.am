JPEG_PATH = $(top_srcdir)/jpeg2

AM_CFLAGS = -g -O3 \
            -fno-short-enums \
            -D_ANDROID_ \
	    -DCODEC_V1 \
	    -DNEW_LOG_API \
            -I$(JPEG_PATH)/src/os \
            -I$(JPEG_PATH)/inc \
            -I$(JPEG_PATH)/src \
            -I$(CAMERAHAL_PATH)/mm-image-codec/qexif \
            -I$(COMMON_INCDIR)/fastcv


if USE_FASTCV_OPT
AM_CFLAGS += -DJPEG_USE_FASTCV_OPT
endif

#false for msm8996 & msm8953
if MM_DEBUG 
AM_CFLAGS += -DVERBOSE -D_DEBUG
endif

#false for 8953 & 8996
if USES_GEMINI 
AM_CFLAGS += -DGEMINI_HW_ENCODE
endif

#false for 8953 & 8996
if USES_MERCURY
AM_CFLAGS += -DMERCURY_HW_DECODE
endif

#true for 8953 & 8996
if USES_ARMV7
AM_CFLAGS += -DARM_ARCH_7A
endif

if TARGET_USES_ION
AM_CFLAGS += -DUSE_ION
endif

#Make the shared library (libmmjpeg)
lib_LTLIBRARIES = libmmjpeg.la
libmmjpeg_la_SOURCES = \
           $(JPEG_PATH)/src/jpege_engine_sw.c \
           $(JPEG_PATH)/src/jpege_engine_sw_huff.c \
           $(JPEG_PATH)/src/jpege_engine_sw_scale_up.c \
           $(JPEG_PATH)/src/jpege_engine_sw_scale_down.c \
           $(JPEG_PATH)/src/jpege_engine_sw_scale.c \
           $(JPEG_PATH)/src/jpege_engine_bs.c \
           $(JPEG_PATH)/src/jpeg_q5_helper_sp.c \
           $(JPEG_PATH)/src/jpeg_writer.c \
           $(JPEG_PATH)/src/jpeg_file_size_control.c \
           $(JPEG_PATH)/src/jpeg_reader.c \
           $(JPEG_PATH)/src/jpeg_buffer.c \
           $(JPEG_PATH)/src/jpeg_header.c \
           $(JPEG_PATH)/src/jpeg_debug.c \
           $(JPEG_PATH)/src/jpeg_queue.c \
	   $(JPEG_PATH)/src/exif.c \
           $(JPEG_PATH)/src/exif_defaults.c \
           $(JPEG_PATH)/src/os/os_pmem_sp.c \
           $(JPEG_PATH)/src/os/os_thread_sp.c \
           $(JPEG_PATH)/src/os/os_timer_sp.c \
           $(JPEG_PATH)/src/writer_utility.c \
	   $(JPEG_PATH)/src/jpege.c \
           $(JPEG_PATH)/src/jpege_engine_hybrid.c \
           $(JPEG_PATH)/src/jpege_englist_sw_only.c \
           $(JPEG_PATH)/src/jpsd.c \
           $(JPEG_PATH)/src/asm/armv7/jpege_engine_sw_dct_fetch_dct_armv7_gcc.S \
           $(JPEG_PATH)/src/asm/armv7/jpege_engine_sw_huff_bs_arm_gcc.S \
           $(JPEG_PATH)/src/asm/armv7/jpege_engine_sw_quant_zigzag_arm_gcc.S


libmmjpeg_la_CFLAGS = $(AM_CFLAGS) -include stdint.h -include stddef.h
libmmjpeg_la_LDFLAGS = -Wl,--no-fatal-warnings

if USE_FASTCV_OPT
libmmjpeg_la_LIBADD = -lcutils -lfastcvopt
else
libmmjpeg_la_LIBADD = -lcutils
endif

if USES_GEMINI
libmmjpeg_la_LIBADD += ../../gemini/libgemini.la
endif

if USES_MERCURY
libmmjpeg_la_LIBADD += ../../mercury/libmercury.la
endif

libmmjpeg_la_LIBADD += -llog -lutils

libmmjpeg_la_LIBADD += $(JPEG_PATH)/lib/os/armv7/libmmjpeg-enc-rvct.a
#libmmjpeg_la_LIBADD += $(JPEG_PATH)/lib/os/armv7/libmmjpeg-dec-rvct.a
