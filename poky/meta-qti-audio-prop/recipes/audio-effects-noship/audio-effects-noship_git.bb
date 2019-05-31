inherit qcommon qprebuilt qlicense

DESCRIPTION = "audio-effects Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-noship/audio-effects/"
S = "${WORKDIR}/audio/mm-audio-noship/audio-effects/"

DEPENDS = "libcutils system-media audio-effects-external-noship"

EXTRA_OECONF += "MM_AUDIO_ENABLED_SAFX=true"
EXTRA_OECONF += "AUDIO_FEATURE_ENABLED_AUDIOSPHERE=true"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
