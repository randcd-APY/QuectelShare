inherit qcommon qprebuilt qlicense

DESCRIPTION = "audio-codecs-noship Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-noship/audio-codecs/"
S = "${WORKDIR}/audio/mm-audio-noship/audio-codecs/"

DEPENDS = "libcutils audio-codecs-ms11-external-noship common"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
