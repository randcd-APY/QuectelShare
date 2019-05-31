AM_CFLAGS = -g -O2 \
            -D_ANDROID_ \
            -include QIDbg.h \
            -DLOGE=ALOGE \
            -DCODEC_V1 \
            -I$(top_srcdir)/codec_v1/common \
            -I$(top_srcdir)/codec_v1/utils \
            -I$(top_srcdir)/codec_v1/encoder \
            -I$(top_srcdir)/codec_v1/decoder \
            -I$(top_srcdir)/codec_v1/exif \
            -I$(top_srcdir)/codec_v1/dma \
            -I$(top_srcdir)/codec_v1/qcrypt \
            -I$(top_srcdir)/jpeg2/src \
            -I$(top_srcdir)/jpeg2/src/os \
            -I$(top_srcdir)/jpeg2/inc \
            -I$(top_srcdir)/jpeg_hw_10/jpegdma_hw/inc \
            -I$(CAMERAHAL_PATH)/mm-image-codec/qexif

if CODECB
AM_CFLAGS += -DCODEC_B
endif
if MSM8953
AM_CFLAGS += -DCODEC_BR
endif
AM_CPPFLAGS = $(AM_CFLAGS)

bin_PROGRAMS = mm-qjpeg-enc-test mm-qjpeg-dec-test qmakernote-xtract qjpeg-dma-test
#encoder
mm_qjpeg_enc_test_SOURCES = QJpegEncoderTest.cpp
mm_qjpeg_enc_test_LDADD = ../libmmqjpeg_codec.la -lcutils -ldl

#decoder
mm_qjpeg_dec_test_SOURCES = QJpegDecoderTest.cpp
mm_qjpeg_dec_test_LDADD = ../libmmqjpeg_codec.la -lcutils -ldl

#makernote data extractor
qmakernote_xtract_SOURCES = QJpegMakernoteExtract.cpp
qmakernote_xtract_LDADD = ../libmmqjpeg_codec.la -lcutils -ldl

#jpeg dma test
qjpeg_dma_test_SOURCES = QJpegDmaTest.cpp
qjpeg_dma_test_LDADD = ../libmmqjpeg_codec.la -lcutils -ldl