# Makefile.am - Automake script for qcril_qmi
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = ipc mcm_log_util mcm_ssr_util client server mcmtest mcm_sim_test \
mcm_mobileap/server/src mcm_mobileap/client/src \
mcm_data mcm_data/tests mcm_atcop/server/src \
mcm_atcop/client/src

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mcm-core.pc
EXTRA_DIST = $(pkgconfig_DATA)
