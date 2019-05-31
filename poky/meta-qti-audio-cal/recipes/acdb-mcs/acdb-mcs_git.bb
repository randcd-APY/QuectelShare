inherit qcommon qlicense qprebuilt

DESCRIPTION = "acdb-mcs Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-cal/audio-acdb-util/acdb-mcs/"
S = "${WORKDIR}/audio/mm-audio-cal/audio-acdb-util/acdb-mcs"

DEPENDS = "acdbloader tinyalsa"
DEPENDS_append = "${@bb.utils.contains('DISTRO_FEATURES', 'audio-dlkm', '', '', d)}"

EXTRA_OECONF_append = " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF_append = " --with-sanitized-headers-audio=${STAGING_KERNEL_BUILDDIR}/usr/techpack/audio/include"
