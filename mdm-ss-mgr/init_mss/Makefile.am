M_CFLAGS = -Wall \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = init_mss

pkgconfigdir = $(libdir)/pkgconfig

init_mss_SOURCES = init_mss.c

init_mss_CFLAGS  = $(CFLAGS)

if SLEEP_INDEFINITE
init_mss_CFLAGS  += -DSLEEP_INDEFINITE
endif

if HAS_MODEM
init_mss_CFLAGS  += -DSOC_HAS_MODEM
endif

if HAS_WCNSS
init_mss_CFLAGS  += -DSOC_HAS_WCNSS
endif
