DESCRIPTION = "Subsystem restart modules"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

PR = "r0"

SRC_URI = "file://${WORKSPACE}/ss-restart"

S = "${WORKDIR}/${PN}"

inherit autotools

DEPENDS += "virtual/kernel"
DEPENDS += "diag"

EXTRA_OECONF = " --with-glib \
                  --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"

EXTRA_OECONF_append = " --with-common-includes=${STAGING_INCDIR}"

FILES_${PN} += "\
    /usr/bin/*"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
