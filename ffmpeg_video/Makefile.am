# sources and intermediate files are separated

AM_CFLAGS := -g -O0 -Werror
AM_CFLAGS += -D_POSIX_SOURCE -DPOSIX_C_SOURCE=199506L
AM_CFLAGS += -D_XOPEN_SOURCE=600
AM_CFLAGS += -D_ANDROID_
AM_CFLAGS += -Wno-deprecated-declarations
AM_CFLAGS += -Wno-multichar
#AM_CFLAGS += -Wno-mismatched-tags
AM_CFLAGS += -fno-stack-protector
AM_CFLAGS += -DNON_BIONIC
AM_CFLAGS += "-Dstrlcpy=g_strlcpy"
AM_CFLAGS += "-Dstrlcat=g_strlcat"
#AM_CFLAGS += "-Dchar32_t=uint32_t"
#AM_CFLAGS += "-Dchar16_t=uint16_t"
AM_CFLAGS += -DNATIVE_WINDOW_SINK_DISABLE
AM_CFLAGS += -DMDP_OVERLAY_SINK_DISABLE
AM_CFLAGS += -DMDP_SOURCE_DISABLE
AM_CFLAGS += -DDISABLE_MBICONVERTOR

AM_CFLAGS += -Istdint.h
AM_CFLAGS += -Iglibconfig.h
AM_CFLAGS += -Iglib.h
AM_CFLAGS += -Istdlib.h

h_sources  =  -I$(top_srcdir)/
AM_CPPFLAGS = $(AM_CFLAGS)
#AM_CPPFLAGS += "-std=c++11"
AM_CPPFLAGS += "-Dstrlcpy=g_strlcpy"
AM_CPPFLAGS += "-Dstrlcat=g_strlcat"


AM_CFLAGS += -Itime.h
AM_CFLAGS += -D__need_timespec
AM_CFLAGS += -D_POSIX_C_SOURCE=199309
AM_CFLAGS += -DEINVAL=22

cpp_sources := encode_video.c
#cpp_sources := main.cpp
#cpp_sources := decode_video.c
#cpp_sources := qcamera_mp4.c 
#cpp_sources +=  utils/src/vt_semaphore.c

bin_PROGRAMS = encode_video

encode_video_SOURCES = $(cpp_sources)
encode_video_CFLAGS = $(h_sources)
encode_video_CFLAGS  += $(AM_CFLAGS)
encode_video_CPPFLAGS = $(h_sources)
encode_video_CPPFLAGS += $(AM_CPPFLAGS)
encode_video_CXXFLAGS = $(h_sources) "-std=c++11"
encode_video_CXXFLAGS += $(AM_CPPFLAGS)
encode_video_LDFLAGS = -lavformat -lavcodec -lavutil -lavdevice -pthread


cpp_sources := decode_video.c
bin_PROGRAMS += decode_video

decode_video_SOURCES = $(cpp_sources)
decode_video_CFLAGS = $(h_sources)
decode_video_CFLAGS  += $(AM_CFLAGS)
decode_video_CPPFLAGS = $(h_sources)
decode_video_CPPFLAGS += $(AM_CPPFLAGS)
decode_video_CXXFLAGS = $(h_sources) "-std=c++11"
decode_video_CXXFLAGS += $(AM_CPPFLAGS)
decode_video_LDFLAGS = -lavformat -lavcodec -lavutil -lavdevice -pthread

