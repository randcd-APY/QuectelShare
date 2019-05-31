inherit qcommon qlicense update-rc.d
DESCRIPTION = "Start up script for mssboot"

SRC_URI ="file://start_mssboot"
S = "${WORKDIR}"
SRC_DIR = "${THISDIR}"

PR = "r2"

INITSCRIPT_NAME = "mssboot"
INITSCRIPT_PARAMS = "start 33 S ."

do_compile[noexec] = "1"

do_install() {
        install -m 0755 ${WORKDIR}/start_mssboot -D ${D}${sysconfdir}/init.d/${INITSCRIPT_NAME}
}
