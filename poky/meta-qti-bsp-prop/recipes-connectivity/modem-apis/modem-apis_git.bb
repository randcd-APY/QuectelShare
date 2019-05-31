inherit autotools

include ${PN}-${MACHINE}.inc
include recipes/oncrpc/oncrpc_defines.inc

DESCRIPTION = "MSM7627A Modem APIs"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
${LICENSE};md5=92b1d0ceea78229551577d4284669bb8"
PR = "r1"

DEPENDS = "msm7k oncrpc glib-2.0"

CFLAGS_prepend = "${oncrpc_defines} ${oncrpc_common_defines} ${defines_api_features}"

EXTRA_OECONF = "--with-glib"
