inherit autotools

include recipes/oncrpc/oncrpc_defines.inc

DESCRIPTION = "ONCRPC Library"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

PR = "r0"

DEPENDS = "diag"

CFLAGS_prepend = "${oncrpc_defines} ${oncrpc_common_defines}"

EXTRA_OECONF = "--with-common-includes=${STAGING_INCDIR}"

SRC_URI = "file://${WORKSPACE}/oncrpc"

S = "${WORKDIR}/oncrpc"
