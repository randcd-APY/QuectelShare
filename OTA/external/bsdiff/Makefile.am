ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS := \
              -I. \
              -I$(WORKSPACE)/OTA/external/libdivsufsort/android_include \
              -D_FILE_OFFSET_BITS=64 \
              -std=c++11

bsdiff_SOURCES := bsdiff.cc bsdiff_main.cc
bsdiff_CPPFLAGS := $(AM_CPPFLAGS)
bsdiff_LDFLAGS :=
bsdiff_LDADD := -lbz2 -ldivsufsort

bspatch_SOURCES := bspatch.cc extents.cc extents_file.cc file.cc memory_file.cc bspatch_main.cc
bspatch_CPPFLAGS := $(AM_CPPFLAGS)
bspatch_LDFLAGS :=
bspatch_LDADD := -lbz2 -ldivsufsort

bin_PROGRAMS := bsdiff bspatch
