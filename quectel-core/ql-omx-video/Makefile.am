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

AM_CPPFLAGS = $(AM_CFLAGS)
#AM_CPPFLAGS += "-std=c++11"
AM_CPPFLAGS += "-Dstrlcpy=g_strlcpy"
AM_CPPFLAGS += "-Dstrlcat=g_strlcat"
AM_CPPFLAGS += -I$(top_srcdir)/inc


AM_CFLAGS += -Itime.h
AM_CFLAGS += -D__need_timespec
AM_CFLAGS += -D_POSIX_C_SOURCE=199309
AM_CFLAGS += -DEINVAL=22

ql_omx_video_include_HEADERS = inc/ql_omx_video.h
ql_omx_video_includedir = $(includedir)

cpp_sources ==  src/QCamera2App.cpp

bin_PROGRAMS = qcamera-omx-test

qcamera_omx_test_SOURCES = $(cpp_sources)
qcamera_omx_test_CFLAGS = $(h_sources)
qcamera_omx_test_CFLAGS  += $(AM_CFLAGS)
qcamera_omx_test_CPPFLAGS = $(h_sources)
qcamera_omx_test_CPPFLAGS += $(AM_CPPFLAGS)
qcamera_omx_test_CXXFLAGS = $(h_sources) "-std=c++11"
qcamera_omx_test_CXXFLAGS += $(AM_CPPFLAGS)
qcamera_omx_test_LDFLAGS = -pthread -lcutils -ldl -L$(top_srcdir)/lib -lqcamera-omx
#libqcamera-omx_LDFLAGS += -lfastcrc -lstreamparser
