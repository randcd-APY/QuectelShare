inherit qcommon qprebuilt qlicense

DESCRIPTION = "acdb rtac Library"
PR = "r3"

DEPENDS = "glib-2.0 audioalsa audcal"
DEPENDS_append = "${@bb.utils.contains('DISTRO_FEATURES', 'audio-dlkm', '', '', d)}"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-cal/audio-acdb-util/acdb-rtac/"
S = "${WORKDIR}/audio/mm-audio-cal/audio-acdb-util/acdb-rtac"

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                 --with-sanitized-headers-audio=${STAGING_KERNEL_BUILDDIR}/usr/techpack/audio/include \
                 --with-glib \
                 --enable-target=${BASEMACHINE}"
