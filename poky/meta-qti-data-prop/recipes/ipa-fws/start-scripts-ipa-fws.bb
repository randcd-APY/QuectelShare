DESCRIPTION = "Script to trigger loading of IPA FWs"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

SRC_URI +="file://ipa_fws.sh"

PR = "r2"

inherit update-rc.d

INITSCRIPT_NAME = "ipa_fws.sh"
INITSCRIPT_PARAMS = "start 30 S ."

do_install() {
    install -m 0755 ${WORKDIR}/${INITSCRIPT_NAME} -D ${D}${sysconfdir}/init.d/${INITSCRIPT_NAME}
}
