inherit qcommon qprebuilt qlicense

DESCRIPTION = "audio-parsers Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/audio-parsers/"
S = "${WORKDIR}/audio/mm-audio/audio-parsers/"

DEPENDS_append_apq8098 = "dtshd-packetizer"
EXTRA_OECONF_append_apq8098 = " AUDIO_FEATURE_ENABLED_DTSHD_PARSER=true"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
