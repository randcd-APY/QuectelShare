# Build libcamera_metadata.so
lib_LTLIBRARIES = libcamera_metadata.la

libcamera_metadata_la_SOURCES := src/camera_metadata.c

libcamera_metadata_la_CFLAGS := -I $(top_srcdir)/include
libcamera_metadata_la_CFLAGS += -I $(top_srcdir)/include/private/camera
libcamera_metadata_la_CFLAGS += -std=c99

# Export headers
camera_metadata_includedir=$(includedir)/system
camera_metadata_include_HEADERS := include/system/camera_metadata.h
camera_metadata_include_HEADERS += include/system/camera_metadata_tags.h
camera_metadata_include_HEADERS += include/system/camera_vendor_tags.h

camera_metadata_private_includedir=$(includedir)/camera
camera_metadata_private_include_HEADERS := include/private/camera/camera_metadata_hidden.h
