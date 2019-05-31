DESCRIPTION = "secure-gst"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/${LICENSE};md5=89aea4e17d99a7cacdbeed46a0096b10"
PR = "r0"

DEPENDS = "securemsm"

SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/gst-media/secure/"
S = "${WORKDIR}/vendor/qcom/proprietary/gst-media/secure/"

PACKAGES = "${PN} ${PN}-dev ${PN}-dbg"
FILES_${PN} += "${libdir}/lib*.so"

inherit autotools qcommon gettext

CFLAGS += "-I${STAGING_KERNEL_BUILDDIR}/usr/include"