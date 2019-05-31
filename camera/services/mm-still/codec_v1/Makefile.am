CURRENT_DIR = $(top_srcdir)/codec_v1
#--- CHECK --- where is this AMSS_VERSION defined?
AM_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION) \
            -g -O2 \
            -D_ANDROID_ \
            -Werror \
            -DCODEC_V1 \
            -DLOGE=ALOGE \
            -include QIDbg.h \
            -I$(CURRENT_DIR)/common \
            -I$(CURRENT_DIR)/dma \
            -I$(CURRENT_DIR)/encoder \
            -I$(CURRENT_DIR)/decoder \
            -I$(CURRENT_DIR)/utils \
            -I$(top_srcdir)/jpeg2/src \
            -I$(top_srcdir)/jpeg2/src/os \
            -I$(top_srcdir)/jpeg2/inc \
            -I$(top_srcdir)/jpeg_hw_10/jpegdma_hw/inc \
            -I$(top_srcdir)/jpeg_hw_10/jpege_hw/inc \
            -I$(top_srcdir)/jpeg_hw_10/jpegd_hw/inc \
            -I$(CAMERAHAL_PATH)/mm-image-codec/qexif \
            -I$(CAMERAHAL_PATH)/QCamera2/stack/common \
            -I$(MM_CAMERA_PATH)/includes \
            -I$(MM_CAMERA_PATH)/media-controller/includes \
            -I$(MM_CAMERA_PATH)/media-controller/modules/imglib/components/include \
            -I$(MM_CAMERA_PATH)/media-controller/modules/imglib/components/lib \
            -I$(MM_CAMERA_PATH)/media-controller/mct/stream \
            -I$(MM_CAMERA_PATH)/media-controller/mct/object \
            -I$(MM_CAMERA_PATH)/media-controller/mct/tools \
            -I$(MM_CAMERA_PATH)/media-controller/mct/event \
            -I$(MM_CAMERA_PATH)/media-controller/mct/module \
            -I$(MM_CAMERA_PATH)/media-controller/mct/bus \
            -I$(MM_CAMERA_PATH)/media-controller/mct/port

if CODECB
AM_CFLAGS += -DCODEC_B
endif
if USE_FASTCV_OPT
AM_CFLAGS += -DJPEG_USE_FASTCV_OPT
endif
if MSM8610
AM_CFLAGS += -DJPEG_USE_QDSP6_ENCODER
endif
if MSM8998
AM_CFLAGS += -DCODEC_HB_V2
AM_CFLAGS += -DJPEG_USE_DMA_V4L2=1
else
AM_CFLAGS += -DJPEG_USE_DMA_V4L2=0
endif

AM_CFLAGS += -include sys/ioctl.h

lib_LTLIBRARIES = libmmqjpeg_codec.la
libmmqjpeg_codec_la_SOURCES = common/QIBase.cpp \
                              common/QIPlane.cpp \
                              common/QImage.cpp \
                              common/QIParams.cpp \
                              common/QIThread.cpp \
                              common/QIBuffer.cpp \
                              common/QIHeapBuffer.cpp \
                              common/QIONBuffer.cpp \
                              common/QITime.cpp \
                              utils/QINode.cpp \
                              utils/QIQueue.cpp \
                              encoder/QEncodeParams.cpp \
                              encoder/QImageSWEncoder.cpp \
                              decoder/QIDecoderParams.cpp \
                              exif/QExifComposerParams.cpp \
                              exif/QExifComposer.cpp \
                              exif/QExifParser.cpp \
                              qcrypt/QCrypt.cpp \
                              exif/QExifCameraTuningParams.cpp \
                              common/QImageDMAFactory.cpp
libmmqjpeg_codec_la_CPPFLAGS = $(AM_CFLAGS)
libmmqjpeg_codec_la_LIBADD = -ldl -lcutils -llog -lcrypto dma/libmmqjpegdma.la jpeg_sw/libmmjpeg.la

if USE_FASTCV_OPT
libmmqjpeg_codec_la_LIBADD += -lfastcvopt
endif
if CODECB
libmmqjpeg_codec_la_SOURCES += common/QImageCodecFactoryBR.cpp \
                               common/QImageCodecFactoryB.cpp \
                               common/QImageCodecFactoryHB_v2.cpp \
                               encoder/QImageHWEncoder10.cpp \
                               decoder/QImageHWDecoder10.cpp \
                               encoder/QImageHybridEncoder.cpp
if USE_PERF_LOCK
libmmqjpeg_codec_la_SOURCES += common/QIPerf.c
libmmqjpeg_codec_la_CPPFLAGS += -DUSE_PERFORMANCE_LOCK
endif
if MSM8610
libmmqjpeg_codec_la_SOURCES += encoder/QImageQ6Encoder.cpp
libmmqjpeg_codec_la_LIBADD += -ladsprpc
endif
else #not codecB
libmmqjpeg_codec_la_SOURCES += common/QImageCodecFactoryA.cpp \
                               encoder/QImageGeminiEncoder.cpp
libmmqjpeg_codec_la_LIBADD += ../gemini/libgemini.la
endif

if TARGET_ENABLE_INSTRUMENTATION
libmmqjpeg_codec_la_CPPFLAGS += -DTARGET_ENABLE_INSTRUMENTATION
libmmqjpeg_codec_la_CPPFLAGS += --coverage
libmmqjpeg_codec_la_LIBADD += -lgcov
libmmqjpeg_codec_la_LDFLAGS = --coverage
endif
