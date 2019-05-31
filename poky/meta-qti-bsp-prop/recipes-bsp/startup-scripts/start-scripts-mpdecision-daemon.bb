DESCRIPTION = "Start up script for MP Decision Daemon"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

SRC_URI +="file://start_mpdecision-daemon"

PR = "r2"

inherit update-rc.d

INITSCRIPT_NAME = "mpdecision"
INITSCRIPT_PARAMS = "start 75 2 3 4 5 . stop 25 0 1 6 ."

do_install_append() {
        install -m 0755 ${WORKDIR}/start_mpdecision-daemon -D ${D}${sysconfdir}/init.d/${INITSCRIPT_NAME}
}

pkg_postinst_${PN} () {
        [ -n "$D" ] && OPT="-r $D" || OPT="-s"
        # remove all rc.d-links potentially created from alternatives
        update-rc.d $OPT -f ${INITSCRIPT_NAME} remove
        update-rc.d $OPT ${INITSCRIPT_NAME} ${INITSCRIPT_PARAMS}
}
