CURRENT_DIR = $(top_srcdir)/jpeg2

AM_CFLAGS = -g -O3 \
            -fno-short-enums \
            -D_ANDROID_ \
            -D_DEBUG \
            -I$(CURRENT_DIR)/src/os \
            -I$(CURRENT_DIR)/inc \
            -I$(CURRENT_DIR)/src \
            -I$(CAMERAHAL_PATH) \
            -I$(top_srcdir)/jpeg_hw_10/jpegd_hw/inc \
            -I$(top_srcdir)/jpeg_hw_10/jpege_hw/inc

if MM_DEBUG
AM_CFLAGS += -DVERBOSE -DMM_DEBUG
endif
if USES_GEMINI
AM_CFLAGS += -DUSE_GEMINI
endif
if USES_MERCURY
AM_CFLAGS += -DUSE_MERCURY
endif
if USES_ARMV7
AM_CFLAGS += -DARM_ARCH_7A
endif
if SMIPOOL_AVAILABLE
AM_CFLAGS += -DSMIPOOL_AVAILABLE
endif
if TARGET_USES_ION
AM_CFLAGS += -DUSE_ION
endif
if NEW_LOG_API
AM_CFLAGS += -DNEW_LOG_API
endif

#Make the shared library (libmmjpeg)
lib_LTLIBRARIES = libmmjpeg.la
libmmjpeg_la_SOURCES = src/jpege.c \
                       src/jpegd.c \
                       src/jpege_engine_hybrid.c \
                       src/jpege_engine_q6.c \
                       src/apr_pmem.c \
                       src/jpege_engine_sw.c \
                       src/jpege_engine_sw_huff.c \
                       src/jpege_engine_sw_scale_up.c \
                       src/jpege_engine_sw_scale_down.c \
                       src/jpege_engine_sw_scale.c \
                       src/jpege_engine_bs.c \
                       src/jpegd_engine_utils.c \
                       src/jpegd_engine_hw_utils.c \
                       src/jpegd_engine_sw.c \
                       src/jpegd_engine_sw_progressive.c \
                       src/jpegd_engine_sw_utils.c \
                       src/jpegd_engine_sw_idct.c \
                       src/jpegd_engine_sw_huff.c \
                       src/jpeg_q5_helper_sp.c \
                       src/jpeg_writer.c \
                       src/jpeg_file_size_control.c \
                       src/jpeg_reader.c \
                       src/jpeg_buffer.c \
                       src/jpeg_header.c \
                       src/jpeg_debug.c \
                       src/jpeg_postprocessor.c \
                       src/jpeg_postprocess_config.c \
                       src/jpeg_postprocess_dm.c \
                       src/jpeg_postprocess_cc.c \
                       src/jpeg_postprocess_ds.c \
                       src/jpeg_postprocess_yuv2rgb.c \
                       src/jpeg_queue.c \
                       src/exif.c \
                       src/exif_defaults.c \
                       src/os/os_pmem_sp.c \
                       src/os/os_thread_sp.c \
                       src/os/os_timer_sp.c \
                       src/writer_utility.c \
                       src/jpegd_englist_sw_only.c \
                       src/jpsd.c

#--- CHECK --- multiple declaration error on uncommenting below file (part of above list after jpege_engine_sw.c)
#libmmjpeg_la_SOURCES += src/jpege_engine_sw_fetch_dct.c

# Add platform-specific source files
if Q5_SW
libmmjpeg_la_SOURCES += src/jpege_englist_q5_sw.c
endif
if SW_ONLY
libmmjpeg_la_SOURCES += src/jpege_englist_sw_only.c
endif
if HW_SW
libmmjpeg_la_SOURCES += src/jpege_englist_hw_sw.c
libmmjpeg_la_SOURCES += src/jpegd_englist_hw_sw.c
if USES_GEMINI
libmmjpeg_la_SOURCES += src/jpege_engine_hw.c
else
libmmjpeg_la_SOURCES += src/jpege_engine_hw_10.c
endif
if USES_MERCURY
libmmjpeg_la_SOURCES += src/jpegd_engine_hw.c
else
libmmjpeg_la_SOURCES += src/jpegd_engine_hw_10.c
endif
endif

libmmjpeg_la_CFLAGS = $(AM_CFLAGS) -include stdint.h -include stddef.h
libmmjpeg_la_LDFLAGS = -Wl,--no-fatal-warnings
libmmjpeg_la_LIBADD = -lcutils
libmmjpeg_la_LIBADD += $(CURRENT_DIR)/lib/os/armv7/libmmjpeg-enc-rvct.a
libmmjpeg_la_LIBADD += $(CURRENT_DIR)/lib/os/armv7/libmmjpeg-dec-rvct.a

if MM_DEBUG
libmmjpeg_la_LIBADD += -llog -lutils
endif
if USES_GEMINI
libmmjpeg_la_LIBADD += ../gemini/libgemini.la
else
libmmjpeg_la_LIBADD += ../jpeg_hw_10/jpege_hw/libjpegehw.la
endif
if USES_MERCURY
libmmjpeg_la_LIBADD += ../mercury/libmercury.la
else
libmmjpeg_la_LIBADD += ../jpeg_hw_10/jpegd_hw/libjpegdhw.la
endif

#Make the tests (mm-jpeg-enc-test and mm-jpeg-dec-test)
#--- CHECK --- uncomment below tests after resolving errors with undefined references to some functions
#bin_PROGRAMS = mm-jpeg-enc-test mm-jpeg-dec-test
#mm_jpeg_enc_test_SOURCES = test/encoder_test.c \
#                           test/ppf_jpeg_header.c
#mm_jpeg_enc_test_CFLAGS = $(AM_CFLAGS)
#if USES_GEMINI
#mm_jpeg_enc_test_CFLAGS += -DGEMINI_HW_ENCODE
#endif
#if USES_MERCURY
#mm_jpeg_enc_test_CFLAGS += -DMERCURY_HW_DECODE
#endif
#mm_jpeg_enc_test_LDADD = -lcutils libmmjpeg.la
#if MM_DEBUG
#mm_jpeg_enc_test_LDADD += -llog -lutils
#endif

#mm_jpeg_dec_test_SOURCES = test/decoder_test.c
#mm_jpeg_dec_test_CFLAGS = $(AM_CFLAGS) -DSCREEN_DUMP_SUPPORTED
#mm_jpeg_dec_test_LDADD = -lcutils libmmjpeg.la
#if MM_DEBUG
#mm_jpeg_dec_test_LDADD  += -llog -lutils
#endif
