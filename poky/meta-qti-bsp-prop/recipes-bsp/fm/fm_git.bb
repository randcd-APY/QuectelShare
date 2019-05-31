inherit autotools
DESCRIPTION = " FM for MSM/QSD"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"
PR = "r0"

SRC_URI = "file://${WORKSPACE}/fm"
S = "${WORKDIR}/fm"

DEPENDS = "qmi-framework glib-2.0"

EXTRA_OECONF_append = " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"

EXTRA_OECONF_append = "${@base_conditional('BASEMACHINE', 'msm8610', ' --enable-target=msm8610', '', d)}"


do_configure[depends] += "virtual/kernel:do_shared_workdir"
