DESCRIPTION = "Common headers required by various QTI recipes"
HOMEPAGE = "http://support.cdmatech.com"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/\
${LICENSE};md5=92b1d0ceea78229551577d4284669bb8"

PR = "r0"

PROVIDES = "common"

FILESPATH =+ "${WORKSPACE}/android_compat/:"
SRC_URI = "file://common/inc/"

S = "${WORKDIR}/common/inc"

do_compile[noexec] = "1"

do_install () {
        install -d ${D}${includedir}
        install -m 0644 ${S}/armasm.h ${D}${includedir}/
        install -m 0644 ${S}/comdef.h ${D}${includedir}/
        install -m 0644 ${S}/customer.h ${D}${includedir}/
        install -m 0644 ${S}/stringl.h ${D}${includedir}/
        install -m 0644 ${S}/target.h ${D}${includedir}/
        install -m 0644 ${S}/common_log.h ${D}${includedir}/
        install -m 0644 ${S}/rex.h ${D}${includedir}/
        install -m 0644 ${S}/msm_ipc.h ${D}${includedir}/
        install -m 0644 ${S}/qsocket.h ${D}${includedir}/
        install -m 0644 ${S}/qsocket_ipcr.h ${D}${includedir}/
}

PACKAGES = "${PN}"
FILES_${PN} += "${includedir}"
