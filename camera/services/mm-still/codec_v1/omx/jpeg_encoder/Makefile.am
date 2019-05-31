AM_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION) \
            -g -O0 \
            -D_ANDROID_ \
            -include QIDbg.h \
            -Werror \
            -DCODEC_V1 \
            -I$(top_srcdir)/codec_v1/common \
            -I$(top_srcdir)/codec_v1/utils \
            -I$(top_srcdir)/codec_v1/encoder \
            -I$(top_srcdir)/codec_v1/decoder \
            -I$(top_srcdir)/codec_v1/omx/common \
            -I$(top_srcdir)/codec_v1/omx/mobicat \
            -I$(top_srcdir)/codec_v1/exif \
            -I$(top_srcdir)/codec_v1/qcrypt \
            -I$(top_srcdir)/codec_v1/dma \
            -I$(top_srcdir)/jpeg2/src \
            -I$(top_srcdir)/jpeg2/src/os \
            -I$(top_srcdir)/jpeg2/inc \
            -I$(CAMERAHAL_PATH)/mm-image-codec/qomx_core \
            -I$(CAMERAHAL_PATH)/mm-image-codec/qexif \
            -I$(CAMERAHAL_PATH)/QCamera2/stack/common \
            -I$(MM_CAMERA_PATH)/media-controller/includes \
            -I$(MM_CAMERA_PATH)/media-controller/mct/stream \
            -I$(MM_CAMERA_PATH)/media-controller/mct/object \
            -I$(MM_CAMERA_PATH)/media-controller/mct/tools \
            -I$(MM_CAMERA_PATH)/media-controller/mct/module \
            -I$(MM_CAMERA_PATH)/media-controller/mct/bus \
            -I$(MM_CAMERA_PATH)/media-controller/mct/event \
            -I$(MM_CAMERA_PATH)/media-controller/mct/port \
            -I$(MM_CAMERA_PATH)/includes \
            -I$(MM_CAMERA_PATH)/server-tuning/tuning \
            -I$(MM_CAMERASDK_PATH)/sensor/includes/$(CHROMATIX_VERSION)

if CAMERA_DEBUG_DATA
AM_CFLAGS += -DCAMERA_DEBUG_DATA
endif

if CODECB
AM_CFLAGS += -DCODEC_B
endif
if MSM8953
AM_CFLAGS += -DCODEC_BR
endif
if MSM8610
AM_CFLAGS += -DJPEG_USE_QDSP6_ENCODER
endif

lib_LTLIBRARIES = libqomx_jpegenc.la libqomx_jpegenc_pipe.la

# Make the shared library (libqomx_encoder)
libqomx_jpegenc_la_SOURCES = OMXImageEncoder.cpp \
                             OMXJpegEncoder.cpp \
                             ../mobicat/QMobicatComposer.cpp \
                             ../common/QOMX_Buffer.cpp \
                             ../common/qomx_core_component.cpp \
                             ../common/QOMXImageCodec.cpp \
                             ../common/QIMessage.cpp


if USE_GLIB
libqomx_jpegenc_la_CPPFLAGS = $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libqomx_jpegenc_la_LDFLAGS = -lpthread @GLIB_LIBS@ -shared
else
libqomx_jpegenc_la_CPPFLAGS = $(AM_CFLAGS)
libqomx_jpegenc_la_LDFLAGS = -lpthread
endif

libqomx_jpegenc_la_CPPFLAGS += -include stdint.h

libqomx_jpegenc_la_LIBADD = -lcutils -ldl ../../libmmqjpeg_codec.la ../../jpeg_sw/libmmjpeg.la
#libqomx_jpegenc_la_LIBADD += ../libqomx_core_helper.a
if MSM8610
libqomx_jpegenc_la_LIBADD += libadsprpc
endif
if TARGET_ENABLE_INSTRUMENTATION
libqomx_jpegenc_la_CPPFLAGS += -DTARGET_ENABLE_INSTRUMENTATION
libqomx_jpegenc_la_CPPFLAGS += --coverage
libqomx_jpegenc_la_LIBADD += -lgcov
libqomx_jpegenc_la_LDFLAGS = --coverage
endif

# Make the shared library (libqomx_jpegenc_pipe)
libqomx_jpegenc_pipe_la_SOURCES = ../common/QOMX_Buffer.cpp \
                                  ../common/QIMessage.cpp \
                                  OMXImageEncoder.cpp \
                                  OMXJpegEncoderPipeline.cpp \
                                  ../common/qomx_core_component.cpp \
                                  ../common/QOMXImageCodec.cpp \
                                  ../mobicat/QMobicatComposer.cpp

if USE_GLIB
libqomx_jpegenc_pipe_la_CPPFLAGS = $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libqomx_jpegenc_pipe_la_LDFLAGS = -lpthread @GLIB_LIBS@
else
libqomx_jpegenc_pipe_la_CPPFLAGS = $(AM_CFLAGS)
libqomx_jpegenc_pipe_la_LDFLAGS = -lpthread
endif

if MSM8998
libqomx_jpegenc_pipe_la_CPPFLAGS += -DJPEG_USE_DMA_V4L2=1
else
libqomx_jpegenc_pipe_la_CPPFLAGS += -DJPEG_USE_DMA_V4L2=0
endif

libqomx_jpegenc_pipe_la_LIBADD = -lcutils -ldl ../../libmmqjpeg_codec.la ../../dma/libmmqjpegdma.la ../../jpeg_sw/libmmjpeg.la
libqomx_jpegenc_pipe_la_LIBADD += libqomx_jpegenc.la ../jpeg_decoder/libqomx_jpegdec.la
if MSM8610
libqomx_jpegenc_pipe_la_LIBADD += libadsprpc
endif
