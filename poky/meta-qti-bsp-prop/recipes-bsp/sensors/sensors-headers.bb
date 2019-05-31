DESCRIPTION = "Installs DSPs sensor headers into staging directory"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"
PV = "1.0.0"
PR = "r3"

SRC_URI = "file://${WORKSPACE}/sensors"

S = "${WORKDIR}/sensors"

PACKAGE_ARCH = "${MACHINE_ARCH}"

do_install_append() {
   mkdir -p ${STAGING_INCDIR}
   mkdir -p ${STAGING_INCDIR}/sensors
   cp -a ${S}/dsps/api/*.h ${STAGING_INCDIR}/sensors/
}

