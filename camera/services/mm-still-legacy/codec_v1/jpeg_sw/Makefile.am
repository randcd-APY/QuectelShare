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

AM_CFLAGS += -DMM_DEBUG=true
AM_CFLAGS += -DMM_DECODER=true
AM_CFLAGS += -DNEW_LOG_API=true
AM_CFLAGS += -DUSE_BIONIC_HEADER=false
AM_CFLAGS += -DUSES_ARMV7=true
AM_CFLAGS += -DJPEG_DEC=q5_sw
AM_CFLAGS += -DJPEG_ENC=sw_only
AM_CFLAGS += -D_DEBUG
AM_CFLAGS += -DARM_ARCH_7A
AM_CFLAGS += -DUSE_ION

#Make the shared library (libmmjpeg)
lib_LTLIBRARIES = libmmjpeg.la
libmmjpeg_la_SOURCES = \
           $(JPEG_PATH)/src/jpege_engine_sw.c \
           $(JPEG_PATH)/src/jpege_engine_sw_huff.c \
           $(JPEG_PATH)/src/jpege_engine_sw_scale_up.c \
           $(JPEG_PATH)/src/jpege_engine_sw_scale_down.c \
           $(JPEG_PATH)/src/jpege_engine_sw_scale.c \
           $(JPEG_PATH)/src/jpege_engine_bs.c \
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
libmmjpeg_la_CFLAGS += -DUSE_PERFORMANCE_LOCK
libmmjpeg_la_LDFLAGS = -Wl,--no-fatal-warnings
libmmjpeg_la_LIBADD = -llog -lutils
libmmjpeg_la_LIBADD += ../../jpeg2/libmmjpeg.la

#bin_PROGRAMS = mm-jpeg-dec-test
#LOCAL_PATH = $(JPEG_PATH)
#mm_jpeg_inc_CFLAGS = -I$(JPEG_PATH)/inc
#mm_jpeg_inc_CFLAGS += -I$(JPEG_PATH)/src
#mm_jpeg_inc_CFLAGS += -I$(JPEG_PATH)/src/os
#mm_jpeg_inc_CFLAGS += -DSCREEN_DUMP_SUPPORTED
#mm_jpeg_inc_SOURCES = test/decoder_test.c
#mm_jpeg_dec_test_LDADD = -llog -lcutils -lmmjpeg

