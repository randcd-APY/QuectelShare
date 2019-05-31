CURRENT_DIR = $(top_srcdir)/codec_v1/omx

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
            -I$(CAMERAHAL_PATH)/mm-image-codec/qomx_core \
            -I$(CAMERAHAL_PATH)/mm-image-codec/qexif \
            -I$(CURRENT_DIR)/common \
            -I$(top_srcdir)/jpeg2/src \
            -I$(top_srcdir)/jpeg2/src/os \
            -I$(top_srcdir)/jpeg2/inc
AM_CFLAGS += -DCODEC_B

AM_CFLAGS += -I$(WORKSPACE)/camera/services/mm-camera-legacy/mm-camera2/includes
AM_CFLAGS += -I$(WORKSPACE)/camera/services/mm-camera-legacy/mm-camera2/media-controller/includes
AM_CFLAGS += -I$(WORKSPACE)/camera/services/mm-camera-legacy/mm-camera2/media-controller/mct/stream
AM_CFLAGS += -I$(WORKSPACE)/camera/services/mm-camera-legacy/mm-camera2/media-controller/mct/object
AM_CFLAGS += -I$(WORKSPACE)/camera/services/mm-camera-legacy/mm-camera2/media-controller/mct/tools
AM_CFLAGS += -I$(WORKSPACE)/camera/services/mm-camera-legacy/mm-camera2/media-controller/mct/event
AM_CFLAGS += -I$(WORKSPACE)/camera/services/mm-camera-legacy/mm-camera2/media-controller/mct/module
AM_CFLAGS += -I$(WORKSPACE)/camera/services/mm-camera-legacy/mm-camera2/media-controller/mct/bus
AM_CFLAGS += -I$(WORKSPACE)/camera/services/mm-camera-legacy/mm-camera2/media-controller/mct/port

AM_CFLAGS += -I$(WORKSPACE)/hardware/qcom/media/mm-core/inc
AM_CFLAGS += -I$(WORKSPACE)/hardware/qcom/camera/mm-image-codec/qomx_core
AM_CFLAGS += -I$(WORKSPACE)/hardware/qcom/camera/mm-image-codec/qexif
AM_CFLAGS += -I$(WORKSPACE)/frameworks/native/include/media/openmax




noinst_LIBRARIES = libqomx_core_helper.a
libqomx_core_helper_a_SOURCES = common/QOMX_Buffer.cpp \
                                common/qomx_core_component.cpp \
                                common/QOMXImageCodec.cpp \
                                common/QIMessage.cpp
libqomx_core_helper_a_CPPFLAGS = $(AM_CFLAGS) -fPIC
