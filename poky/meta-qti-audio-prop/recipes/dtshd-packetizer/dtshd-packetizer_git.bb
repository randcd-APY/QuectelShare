inherit qcommon qprebuilt qlicense

DESCRIPTION = "dtshd-packetizer Library"
PR = "r0"

DEPENDS = "glib-2.0 libcutils"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-noship/audio-parsers/dtshd-packetizer/"
S = "${WORKDIR}/audio/mm-audio-noship/audio-parsers/dtshd-packetizer/"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
