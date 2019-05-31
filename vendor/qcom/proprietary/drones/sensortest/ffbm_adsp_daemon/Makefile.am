ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = ffbm_adsp

# ffbm_adsp_daemon
# =========================================================
ffbm_adsp_SOURCES := \
    daemon.c


AM_CFLAGS = -O2 \
    -g \
    -Wall \
    -Wno-unused-parameter \
    -D_XOPEN_SOURCE \
    -D_GNU_SOURCE \
    -I../include \
    -D__USE_GNU

ffbm_adsp_LDADD = -lz
