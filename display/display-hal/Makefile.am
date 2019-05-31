# Makefile.am - Automake script for sdm

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = sdm/libs/utils
if ENABLE_SDMHALDRM
SUBDIRS += libdrmutils
SUBDIRS += gpu_tonemapper
else
if !ENABLE_SDMHALFB
SUBDIRS += libqservice
SUBDIRS += libqdutils
SUBDIRS += libgralloc
endif
endif
SUBDIRS += sdm/libs/core


