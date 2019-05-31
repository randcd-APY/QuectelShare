AM_CFLAGS = -Wno-missing-field-initializersi -Wall -Wno-unused-parameter -std=c++11
HEADER_PATH := $(top_srcdir)/gpu_tonemapper

cpp_sources = glengine.cpp \
              EGLImageBufferBase.cpp \
              EGLImageWrapperLE.cpp \
              EGLImageBufferLE.cpp \
              TonemapFactoryLE.cpp \
              TonemapperLE.cpp

gpu_tonemapper_h_sources = $(HEADER_PATH)/*.h
gpu_tonemapper_includedir = $(includedir)/opengl/include
gpu_tonemapper_include_HEADERS = $(gpu_tonemapper_h_sources)

lib_LTLIBRARIES = libgpu_tonemapper.la
libgpu_tonemapper_la_CC = @CC@
libgpu_tonemapper_la_SOURCES = $(cpp_sources)
libgpu_tonemapper_la_CFLAGS = $(COMMON_CFLAGS) -DLOG_TAG=\"GPU_TONEMAPPER\"
libgpu_tonemapper_la_CPPFLAGS = $(AM_CPPFLAGS)
libgpu_tonemapper_la_LDFLAGS = -shared -avoid-version
