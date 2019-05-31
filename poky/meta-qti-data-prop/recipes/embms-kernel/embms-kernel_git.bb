inherit autotools-brokensep module update-rc.d qperf
DESCRIPTION = "Embms Kernel Module"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/${LICENSE};md5=f3b90e78ea0cffb20bf5cca7947a896d"

PR = "${@oe.utils.conditional('PRODUCT', 'psm', 'r0-psm', 'r0', d)}"

FILESPATH =+ "${WORKSPACE}:"

SRC_URI = "${@oe.utils.conditional('PREFERRED_VERSION_linux-msm', '4.14', 'file://kernel/msm-4.14/net/embms_kernel/', '', d)} \
           ${@oe.utils.conditional('PREFERRED_VERSION_linux-msm', '4.9', 'file://kernel/msm-4.9/net/embms_kernel/', '', d)} \
           ${@oe.utils.conditional('PREFERRED_VERSION_linux-msm', '3.18', 'file://kernel/msm-3.18/net/embms_kernel/', '', d)} \
           file://start_embms_le"

S = "${@oe.utils.conditional('PREFERRED_VERSION_linux-msm', '4.14', '${WORKDIR}/kernel/msm-4.14/net/embms_kernel/', '', d)} \
     ${@oe.utils.conditional('PREFERRED_VERSION_linux-msm', '4.9', '${WORKDIR}/kernel/msm-4.9/net/embms_kernel/', '', d)} \
     ${@oe.utils.conditional('PREFERRED_VERSION_linux-msm', '3.18', '${WORKDIR}/kernel/msm-3.18/net/embms_kernel/', '', d)}"

FILES_${PN}="/etc/init.d/start_embms_le"

do_install() {
    module_do_install
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/start_embms_le ${D}${sysconfdir}/init.d
}

INITSCRIPT_NAME = "start_embms_le"
INITSCRIPT_PARAMS = "start 35 5 . stop 15 0 1 6 ."
