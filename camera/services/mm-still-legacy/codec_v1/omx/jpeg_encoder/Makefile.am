AM_CFLAGS = -DAMSS_VERSION=$(AMSS_VERSION) \
            -g -O0 \
            -D_ANDROID_ \
            -include QIDbg.h \
            -Werror \
            -DCODEC_V1 \
            -include stdlib.h \
            -include string.h \
            -I . \
            -I$(top_srcdir)/gemini/inc \
            -I$(top_srcdir)/codec_v1/common \
            -I$(top_srcdir)/codec_v1/utils \
            -I$(top_srcdir)/codec_v1/encoder \
            -I$(top_srcdir)/codec_v1/decoder \
            -I$(top_srcdir)/codec_v1/omx/common \
            -I$(top_srcdir)/codec_v1/omx/mobicat \
            -I$(top_srcdir)/codec_v1/exif \
            -I$(top_srcdir)/codec_v1/qcrypt \
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
            -I$(MM_CAMERASDK_PATH)/sensor/includes/0301
AM_CFLAGS += -I$(WORKSPACE)/frameworks/native/include/media/openmax
AM_CFLAGS += -I$(WORKSPACE)/hardware/qcom/camera/mm-image-codec/qomx_core
AM_CFLAGS += -I$(WORKSPACE)/hardware/qcom/camera/mm-image-codec/qexif
AM_CFLAGS += -I$(WORKSPACE)/adsprpc/inc
AM_CFLAGS += -DCODEC_B
AM_CFLAGS += -I$(WORKSPACE)/hardware/qcom/media/mm-core/inc
AM_CFLAGS += -I$(WORKSPACE)/hardware/qcom/camera/mm-image-codec/qomx_core
AM_CFLAGS += -I$(WORKSPACE)/hardware/qcom/camera/mm-image-codec/qexif
AM_CFLAGS += -I$(WORKSPACE)/frameworks/native/include/media/openmax
lib_LTLIBRARIES = libqomx_jpegenc.la
AM_CFLAGS += -fPIC
# Make the shared library (libqomx_encoder)
libqomx_jpegenc_la_SOURCES = OMXImageEncoder.cpp \
                             OMXJpegEncoder.cpp \
                             ../mobicat/QMobicatComposer.cpp \
                             ../common/QOMX_Buffer.cpp \
                             ../common/qomx_core_component.cpp \
                             ../common/QOMXImageCodec.cpp
                             #../commom/QIMessage.cpp
if USE_GLIB
libqomx_jpegenc_la_CPPFLAGS = $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
libqomx_jpegenc_la_LDFLAGS = -lpthread @GLIB_LIBS@
else
libqomx_jpegenc_la_CPPFLAGS = $(AM_CFLAGS)
libqomx_jpegenc_la_LDFLAGS = -lpthread
endif

libqomx_jpegenc_la_CPPFLAGS += -include stdint.h

libqomx_jpegenc_la_LIBADD = -lcutils -llog -ldl ../../libmmqjpeg_codec.la ../../jpeg_sw/libmmjpeg.la
#libqomx_jpegenc_la_LIBADD += ../libqomx_core_helper.a
libqomx_jpegenc_la_LIBADD += ../jpeg_decoder/libqomx_jpegdec.la

