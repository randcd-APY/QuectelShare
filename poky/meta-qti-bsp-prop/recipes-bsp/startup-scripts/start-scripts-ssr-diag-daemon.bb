DESCRIPTION = "Start up script for SSR Diag Daemon"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

SRC_URI +="file://start_ssr-diag-daemon"

PR = "r1"

inherit update-rc.d

INITSCRIPT_NAME = "ssr-diag-daemon"
INITSCRIPT_PARAMS = "start 30 2 3 4 5 . stop 70 0 1 6 ."

do_install() {
        install -m 0755 ${WORKDIR}/start_ssr-diag-daemon -D ${D}${sysconfdir}/init.d/ssr-diag-daemon
}

pkg_postinst_${PN} () {
        [ -n "$D" ] && OPT="-r $D" || OPT="-s"
        # remove all rc.d-links potentially created from alternatives
        update-rc.d $OPT -f ${INITSCRIPT_NAME} remove
        update-rc.d $OPT ${INITSCRIPT_NAME} ${INITSCRIPT_PARAMS}
}
