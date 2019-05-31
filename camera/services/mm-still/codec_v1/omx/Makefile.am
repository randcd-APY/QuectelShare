CURRENT_DIR = $(top_srcdir)/codec_v1/omx

AM_CFLAGS = -g -O2 \
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

if CODECB
AM_CFLAGS += -DCODEC_B
endif
if MSM8953
AM_CFLAGS += -DCODEC_BR
endif
if MSM8998
AM_CFLAGS += -DCODEC_HB_V2
endif

noinst_LIBRARIES = libqomx_core_helper.a
libqomx_core_helper_a_SOURCES = common/QOMX_Buffer.cpp \
                                common/qomx_core_component.cpp \
                                common/QOMXImageCodec.cpp \
                                common/QIMessage.cpp
libqomx_core_helper_a_CPPFLAGS = $(AM_CFLAGS) -fPIC
