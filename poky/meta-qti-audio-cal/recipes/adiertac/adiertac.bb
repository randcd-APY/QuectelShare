inherit qcommon qprebuilt qlicense

DESCRIPTION = "adie rtac Library and calibiration files"
PR = "r3"

DEPENDS = "glib-2.0 audioalsa audcal"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-cal/audio-acdb-util/adie-rtac/"
S = "${WORKDIR}/audio/mm-audio-cal/audio-acdb-util/adie-rtac"

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                 --with-glib \
                 --enable-target=${BASEMACHINE}"
