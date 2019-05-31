AM_CFLAGS = -g -O2 \
            -D_ANDROID_ \
            -include QIDbg.h \
            -Werror \
            -DCODEC_V1 \
            -DLOGE=ALOGE \
            -I$(top_srcdir)/codec_v1/common \
            -I$(top_srcdir)/codec_v1/utils \
            -I$(top_srcdir)/codec_v1/encoder \
            -I$(top_srcdir)/jpeg2/src \
            -I$(top_srcdir)/jpeg2/src/os \
            -I$(top_srcdir)/jpeg2/inc \
            -I$(top_srcdir)/jpeg_hw_10/jpegdma_hw/inc \
            -I$(top_srcdir)/jpeg_hw_10/jpege_hw/inc \
            -I$(top_srcdir)/jpeg_hw_10/jpegd_hw/inc \
            -I$(MM_CAMERA_PATH)/media-controller/modules/imglib/components/include \
            -I$(MM_CAMERA_PATH)/media-controller/modules/imglib/components/lib \
            -I$(CAMERAHAL_PATH)/QCamera2/stack/common \
            -I$(CAMERAHAL_PATH)/mm-image-codec/qexif

if CODECB
AM_CFLAGS += -DCODEC_B
endif

AM_CFLAGS += -include sys/ioctl.h

lib_LTLIBRARIES = libmmqjpegdma.la
libmmqjpegdma_la_SOURCES = ../common/QIBase.cpp \
                           ../common/QIPlane.cpp \
                           ../common/QImage.cpp \
                           ../common/QIParams.cpp \
                           ../common/QIThread.cpp \
                           ../common/QIBuffer.cpp \
                           ../common/QIHeapBuffer.cpp \
                           ../common/QIONBuffer.cpp \
                           ../common/QITime.cpp \
                           ../utils/QINode.cpp \
                           ../utils/QIQueue.cpp \
                           QJPEGDMAEngine.cpp \
                           QJPEGDMA_V4L2_Engine.cpp
libmmqjpegdma_la_CPPFLAGS = $(AM_CFLAGS)
libmmqjpegdma_la_LIBADD = -ldl -lcutils -llog
