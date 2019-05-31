inherit autotools qcommon

DESCRIPTION = "encoders"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/\
${LICENSE};md5=3775480a712fc46a69647678acb234cb"

PR = "r0"

SRC_DIR = "${WORKSPACE}/hardware/qcom/audio/mm-audio/"

S = "${WORKDIR}/hardware/qcom/audio/mm-audio/"
EXTRA_OECONF_append += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF_append += "--with-glib"


do_configure[depends] += "virtual/kernel:do_shared_workdir"

DEPENDS = "glib-2.0 system-core media"

DEPENDS_append = "${@bb.utils.contains('DISTRO_FEATURES', 'audio-dlkm', '', '', d)}"

RDEPENDS_${PN} = "media"

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
