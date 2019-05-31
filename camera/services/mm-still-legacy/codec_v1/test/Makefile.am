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
            -I$(top_srcdir)/codec_v1/qcrypt \
            -I$(top_srcdir)/jpeg2/src \
            -I$(top_srcdir)/jpeg2/src/os \
            -I$(top_srcdir)/jpeg2/inc \
            -I$(top_srcdir)/jpeg_hw_10/jpegdma_hw/inc \
            -I$(CAMERAHAL_PATH)/mm-image-codec/qexif
AM_CFLAGS += -DCODEC_B
AM_CPPFLAGS = $(AM_CFLAGS)

bin_PROGRAMS = mm-qjpeg-enc-test mm-qjpeg-dec-test qmakernote-xtract
#encoder
mm_qjpeg_enc_test_CPPFLAGS = $(AM_CFLAGS)
mm_qjpeg_enc_test_SOURCES = QJpegEncoderTest.cpp
mm_qjpeg_enc_test_LDADD = -llog -lcutils -ldl ../libmmqjpeg_codec.la

#decoder
mm_qjpeg_dec_test_CPPFLAGS = $(AM_CFLAGS)
mm_qjpeg_dec_test_SOURCES = QJpegDecoderTest.cpp
mm_qjpeg_dec_test_LDADD = -llog -lcutils -ldl ../libmmqjpeg_codec.la

#makernote data extractor
qmakernote_xtract_CPPFLAGS = $(AM_CFLAGS)
qmakernote_xtract_SOURCES = QJpegMakernoteExtract.cpp
qmakernote_xtract_LDADD = -llog -lcutils -ldl ../libmmqjpeg_codec.la
