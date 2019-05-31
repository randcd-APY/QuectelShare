AM_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION) \
            -g -O0 \
            -D_ANDROID_ \
            -include QIDbg.h \
            -Werror \
            -DCODEC_V1 \
            -I$(top_srcdir)/codec_v1/common \
            -I$(top_srcdir)/codec_v1/utils \
            -I$(top_srcdir)/codec_v1/omx/common \
            -I$(top_srcdir)/codec_v1/encoder \
            -I$(top_srcdir)/codec_v1/decoder \
            -I$(top_srcdir)/codec_v1/exif \
            -I$(CAMERAHAL_PATH)/mm-image-codec/qomx_core \
            -I$(CAMERAHAL_PATH)/mm-image-codec/qexif \
            -I$(top_srcdir)/jpeg2/src \
            -I$(top_srcdir)/jpeg2/src/os \
            -I$(top_srcdir)/jpeg2/inc

AM_CFLAGS += -DCODEC_B
AM_CFLAGS += -I$(WORKSPACE)/frameworks/native/include/media/openmax
AM_CFLAGS += -I$(WORKSPACE)/hardware/qcom/camera/mm-image-codec/qomx_core
AM_CFLAGS += -I$(WORKSPACE)/hardware/qcom/camera/mm-image-codec/qexif

#Make the shared library (libqomx_decoder)
lib_LTLIBRARIES = libqomx_jpegdec.la
libqomx_jpegdec_la_SOURCES = OMXImageDecoder.cpp \
                             OMXJpegDecoder.cpp
libqomx_jpegdec_la_CPPFLAGS = $(AM_CFLAGS)
libqomx_jpegdec_la_LIBADD = -lcutils -llog -ldl ../../libmmqjpeg_codec.la ../../jpeg_sw/libmmjpeg.la
libqomx_jpegdec_la_LIBADD += ../libqomx_core_helper.a
