inherit autotools

include recipes/modem-apis/modem-apis-${MACHINE}.inc

DESCRIPTION = "ONCRPC Tests"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

PR = "r0"

DEPENDS = "oncrpc modem-apis"

EXTRA_OECONF = "--with-common-includes=${STAGING_INCDIR}"

CFLAGS_prepend = "${oncrpc_defines} ${oncrpc_common_defines} ${defines_api_features}"

SRC_URI = "file://${WORKSPACE}/oncrpc/test"

S = "${WORKDIR}/test"
