AM_CFLAGS = -DCAMERA_ION_HEAP_ID=ION_CP_MM_HEAP_ID \
            -D_ANDROID_ \
            -include QIDbg.h \
            -I$(top_srcdir)/codec_v1/common \
            -I$(CAMERAHAL_PATH)/mm-image-codec/qomx_core \
            -I$(CAMERAHAL_PATH)/mm-image-codec/qexif \
            -I$(top_srcdir)/utils \
            -I$(top_srcdir)/exit \
            -I$(top_srcdir)/commom \
            -I .

AM_CFLAGS += -I$(WORKSPACE)/frameworks/native/include/media/openmax
AM_CFLAGS += -I$(WORKSPACE)/hardware/qcom/camera/mm-image-codec/qomx_core
AM_CFLAGS += -I$(WORKSPACE)/hardware/qcom/camera/mm-image-codec/qexif

AM_CFLAGS += -DUSE_ION

bin_PROGRAMS = mm-qomx-ienc-test mm-qomx-idec-test
#encoder
mm_qomx_ienc_test_CFLAGS = $(AM_CFLAGS)
mm_qomx_ienc_test_SOURCES = buffer_test.c \
                            qomx_jpeg_enc_test.c
mm_qomx_ienc_test_LDADD = -llog -lcutils -ldl -lqomx_core

#decoder
mm_qomx_idec_test_CFLAGS = $(AM_CFLAGS)
mm_qomx_idec_test_SOURCES = buffer_test.c \
                            qomx_jpeg_dec_test.c
mm_qomx_idec_test_LDADD = -llog -lcutils -ldl -lqomx_core
