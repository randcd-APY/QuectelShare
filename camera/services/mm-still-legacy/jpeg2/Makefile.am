CURRENT_DIR = $(top_srcdir)/jpeg2

AM_CFLAGS = -g -O3 \
            $(KERNEL_HEADERS:%=-I%) \
            -fno-short-enums \
            -D_ANDROID_ \
            -D_DEBUG \
            -I$(CURRENT_DIR)/src/os \
            -I$(CURRENT_DIR)/inc \
            -I$(CURRENT_DIR)/src \
            -I$(CAMERAHAL_PATH) \
            -I$(top_srcdir)/jpeg_hw_10/jpegd_hw/inc \
            -I$(top_srcdir)/jpeg_hw_10/jpege_hw/inc
AM_CFLAGS += -I$(WORKSPACE)/cv/fastcv-noship/inc/fastcv
AM_CFLAGS += -I$(PKG_CONFIG_SYSROOT_DIR)/usr/include/fastcv
AM_CFLAGS += -DUSE_ION
AM_CFLAGS += -DMM_DEBUG=false
AM_CFLAGS += -DMM_DECODER=true
AM_CFLAGS += -include stdint.h
AM_CFLAGS += -include stddef.h

AM_CFLAGS += -I$(top_srcdir)/jpeg


#Make the shared library (libmmjpeg)
lib_LTLIBRARIES = libmmjpeg.la
libmmjpeg_la_SOURCES = src/jpege.c
libmmjpeg_la_SOURCES += src/jpegd.c
libmmjpeg_la_SOURCES += src/jpege_engine_hybrid.c
libmmjpeg_la_SOURCES += src/jpege_engine_q6.c
#libmmjpeg_la_SOURCES += src/apr_pmem.c
libmmjpeg_la_SOURCES += src/jpege_engine_sw.c
libmmjpeg_la_SOURCES += src/jpege_engine_sw_fetch_dct.c
libmmjpeg_la_SOURCES += src/jpege_engine_sw_huff.c
libmmjpeg_la_SOURCES += src/jpege_engine_sw_scale_up.c
libmmjpeg_la_SOURCES += src/jpege_engine_sw_scale_down.c
libmmjpeg_la_SOURCES += src/jpege_engine_sw_scale.c
libmmjpeg_la_SOURCES += src/jpege_engine_bs.c
#libmmjpeg_la_SOURCES += src/jpege_engine_q5.c
libmmjpeg_la_SOURCES += src/jpegd_engine_utils.c
libmmjpeg_la_SOURCES += src/jpegd_engine_hw_utils.c
libmmjpeg_la_SOURCES += src/jpegd_engine_sw.c
libmmjpeg_la_SOURCES += src/jpegd_engine_sw_progressive.c
libmmjpeg_la_SOURCES += src/jpegd_engine_sw_utils.c
libmmjpeg_la_SOURCES += src/jpegd_engine_sw_idct.c
libmmjpeg_la_SOURCES += src/jpegd_engine_sw_huff.c
#libmmjpeg_la_SOURCES += src/jpegd_engine_q5.c
#libmmjpeg_la_SOURCES += src/jpeg_q5_helper_sp.c
libmmjpeg_la_SOURCES += src/jpeg_writer.c
libmmjpeg_la_SOURCES += src/jpeg_file_size_control.c
libmmjpeg_la_SOURCES += src/jpeg_reader.c
libmmjpeg_la_SOURCES += src/jpeg_buffer.c
libmmjpeg_la_SOURCES += src/jpeg_header.c
libmmjpeg_la_SOURCES += src/jpeg_debug.c
libmmjpeg_la_SOURCES += src/jpeg_postprocessor.c
libmmjpeg_la_SOURCES += src/jpeg_postprocess_config.c
libmmjpeg_la_SOURCES += src/jpeg_postprocess_dm.c
libmmjpeg_la_SOURCES += src/jpeg_postprocess_cc.c
libmmjpeg_la_SOURCES += src/jpeg_postprocess_ds.c
libmmjpeg_la_SOURCES += src/jpeg_postprocess_yuv2rgb.c
libmmjpeg_la_SOURCES += src/jpeg_queue.c
libmmjpeg_la_SOURCES += src/exif.c
libmmjpeg_la_SOURCES += src/exif_defaults.c
libmmjpeg_la_SOURCES += src/os/os_pmem_sp.c
libmmjpeg_la_SOURCES += src/os/os_thread_sp.c
libmmjpeg_la_SOURCES += src/os/os_timer_sp.c
libmmjpeg_la_SOURCES += src/writer_utility.c
libmmjpeg_la_SOURCES += src/jpegd_englist_sw_only.c
libmmjpeg_la_SOURCES += src/jpsd.c


h_sources = inc/exif.h
h_sources += inc/jpeg_buffer.h
h_sources += inc/jpeg_common.h
h_sources += inc/jpegd.h
h_sources += inc/jpege.h
h_sources += inc/jpegerr.h
h_sources += inc/jps.h
h_sources += inc/jpsd.h
h_sources += inc/os_int.h
h_sources += inc/os_int_sp.h
h_sources += inc/os_types.h
h_sources += inc/os_types_sp.h
h_sources += inc/os_pmem.h
h_sources += inc/os_pmem_sp.h

pkgincludedir = $(includedir)/mm-still-legacy/jpeg
library_includedir = $(pkgincludedir)
library_include_HEADERS = $(h_sources)

libmmjpeg_la_CFLAGS = $(AM_CFLAGS)
libmmjpeg_la_CFLAGS += -DUSE_PERFORMANCE_LOCK
libmmjpeg_la_LDFLAGS = -Wl,--no-fatal-warnings -shared
libmmjpeg_la_LIBADD = -lcutils -llog #-ljpegehw
#libmmjpeg_la_LIBADD += $(CURRENT_DIR)/lib/os/armv7/libmmjpeg-enc-rvct.a
#libmmjpeg_la_LIBADD += $(CURRENT_DIR)/lib/os/armv7/libmmjpeg-dec-rvct.a
libmmjpeg_la_LIBADD += ../jpeg_hw_10/jpegd_hw/libjpegdhw.la


#bin_PROGRAMS = mm-jpeg-enc-test mm-jpeg-dec-test
##mm-jpeg-enc-test
#mm_jpeg_enc_test_CFLAGS = $(AM_CFLAGS)
#mm_jpeg_enc_test_CFLAGS += -I$(CURRENT_DIR)/test
#mm_jpeg_enc_test_SOURCES = test/encoder_test.c
#mm_jpeg_enc_test_SOURCES = test/ppf_jpeg_header.c
#mm_jpeg_enc_test_LDADD = -llog -lcutils -lmmjpeg


##mm-jpeg-dec-test
#mm_jpeg_dec_test_CFLAGS = $(AM_CFLAGS)
#mm_jpeg_dec_test_CFLAGS += -I$(CURRENT_DIR)/test
#mm_jpeg_dec_test_SOURCES = test/decoder_test.c
#mm_jpeg_dec_test_LDADD = -llog -lcutils -lmmjpeg

