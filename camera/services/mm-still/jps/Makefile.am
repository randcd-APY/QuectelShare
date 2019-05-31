CURRENT_DIR = $(top_srcdir)/jps
JPEG2_DIR = $(top_srcdir)/jpeg2

AM_CFLAGS = -g -O3 \
            -fno-short-enums \
            -D_ANDROID_ \
            -D_DEBUG \
            -I$(CURRENT_DIR)/inc \
            -I$(JPEG2_DIR)/src \
            -I$(JPEG2_DIR)/src/os \
            -I$(JPEG2_DIR)/inc \
            -I$(CAMERAHAL_PATH)

if MM_DEBUG
AM_CFLAGS += -DVERBOSE -DMM_DEBUG
endif
if NEW_LOG_API
AM_CFLAGS += -DNEW_LOG_API
endif

# Make the shared library (libmmjps)
lib_LTLIBRARIES = libmmjps.la
libmmjps_la_SOURCES = src/jpse.c \
                      src/jps_writer.c \
                      src/jpse_englist_sw_only.c
libmmjps_la_LIBADD = -lcutils
libmmjps_la_LIBADD += ../jpeg2/libmmjpeg.la
if MM_DEBUG
libmmjps_la_LIBADD += -llog -lutils
endif

# Make the test (mm-jps-enc-test)
bin_PROGRAMS = mm-jps-enc-test
mm_jps_enc_test_SOURCES = test/jpse_test.c \
                          test/ppf_jpeg_header.c
mm_jps_enc_test_CFLAGS = $(AM_CFLAGS)
if USES_GEMINI
mm_jps_enc_test_CFLAGS += -DGEMINI_HW_ENCODE
endif
mm_jps_enc_test_LDADD = -lcutils
mm_jps_enc_test_LDADD += libmmjps.la ../jpeg2/libmmjpeg.la
if MM_DEBUG
mm_jps_enc_test_LDADD += -llog -lutils
endif
