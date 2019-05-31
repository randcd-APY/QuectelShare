inherit qcommon qlicense qprebuilt systemd

DESCRIPTION = "Data Path Optimizer to achieve higher throughputs"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

PR = "r2"

DEPENDS = "configdb dsutils glib-2.0"

S = "${WORKDIR}/data-binary/data_path_opt/"
SRC_DIR = "${WORKSPACE}/data-binary/data_path_opt/"

EXTRA_OECONF = "--with-common-includes=${STAGING_INCDIR} \
                --with-glib"



