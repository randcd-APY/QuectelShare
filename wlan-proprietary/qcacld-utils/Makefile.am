# Makefile.am - Automake script for qcacld-tools

ACLOCAL_AMFLAGS = -I m4

dirs :

dirs = cnssdiag
dirs += athdiag
dirs += pktlog
if NEED_SPECTRAL
dirs += spectral
endif
if !THIRD_PARTY_SUPPORTED
dirs += asf
dirs += ptt
if NEED_SCM
dirs += wlan_scm
endif
dirs += csm
endif
SUBDIRS = $(dirs)
