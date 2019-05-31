ACLOCAL_AMFLAGS = -I m4

dirs =
#dirs += gemini
#dirs += mercury
dirs += jpeg_hw_10/jpegdma_hw
dirs += jpeg_hw_10/jpegd_hw
dirs += jpeg_hw_10/jpege_hw
dirs += jpeg_hw_10/test
#dirs += jpeg2
#dirs += jps
dirs += codec_v1/jpeg_sw
dirs += codec_v1/dma
dirs += codec_v1
#dirs += codec_v1/test
if !DISABLE_MEDIA_OMX
dirs += codec_v1/omx
dirs += codec_v1/omx/jpeg_decoder
dirs += codec_v1/omx/jpeg_encoder
dirs += codec_v1/omx/test
endif
SUBDIRS = $(dirs)
