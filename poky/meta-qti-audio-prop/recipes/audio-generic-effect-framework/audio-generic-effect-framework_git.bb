inherit qcommon qprebuilt qlicense

DESCRIPTION = "audio-generic-effect-framework Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/audio-generic-effect-framework/"
S = "${WORKDIR}/audio/mm-audio/audio-generic-effect-framework/"

DEPENDS = "liblog"
DEPENDS += "libcutils"
DEPENDS += "system-media libhardware"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
