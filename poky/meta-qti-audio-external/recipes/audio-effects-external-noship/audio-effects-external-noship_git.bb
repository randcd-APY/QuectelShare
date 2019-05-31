inherit qcommon qprebuilt qlicense

DESCRIPTION = "audio-effects Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-external-noship/audio-systems/audio-effects/"
S = "${WORKDIR}/audio/mm-audio-external-noship/audio-systems/audio-effects/"

DEPENDS = "libcutils system-media common"

EXTRA_OECONF += "MM_AUDIO_ENABLED_SAFX=true"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"

ALLOW_EMPTY_${PN} = "1"
