inherit qcommon qprebuilt qlicense

DESCRIPTION = "adpcm-dec Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-noship/adpcm-dec/"
S = "${WORKDIR}/audio/mm-audio-noship/adpcm-dec/"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
