ACLOCAL_AMFLAGS = -I m4

CURRENT_DIR = $(top_srcdir)/codec_v1
#--- CHECK --- where is this AMSS_VERSION defined?
AM_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION) \
            -g -O2 \
            -D_ANDROID_ \
            -DCODEC_V1 \
            -DLOGE=ALOGE \
            -include QIDbg.h \
            -include stdlib.h \
            -include string.h \
            -I$(CURRENT_DIR)/common \
            -I$(CURRENT_DIR)/encoder \
            -I$(CURRENT_DIR)/decoder \
            -I$(CURRENT_DIR)/utils \
            -I$(top_srcdir)/gemini/inc \
            -I$(top_srcdir)/jpeg2/src \
            -I$(top_srcdir)/jpeg2/src/os \
            -I$(top_srcdir)/jpeg2/inc \
            -I$(top_srcdir)/jpeg2/test \
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
AM_CFLAGS += -I$(WORKSPACE)/adsprpc/inc
AM_CFLAGS += -DCODEC_B
AM_CFLAGS += -include sys/ioctl.h
AM_CFLAGS += -DUSE_PERFORMANCE_LOCK
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
                              exif/QExifCameraTuningParams.cpp
libmmqjpeg_codec_la_CPPFLAGS = $(AM_CFLAGS)
libmmqjpeg_codec_la_LIBADD = -ldl -lcutils -llog -lcrypto ../jpeg2/libmmjpeg.la
#libmmqjpeg_codec_la_LIBADD += -lfastcvopt -ladsprpc
libmmqjpeg_codec_la_LIBADD += -ladsprpc

libmmqjpeg_codec_la_SOURCES += common/QImageCodecFactoryB.cpp \
                               encoder/QImageHWEncoder10.cpp \
                               decoder/QImageHWDecoder10.cpp

libmmqjpeg_codec_la_SOURCES += common/QIPerf.c
libmmqjpeg_codec_la_SOURCES += encoder/QImageHybridEncoder.cpp

