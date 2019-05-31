inherit qcommon qprebuilt qlicense

DESCRIPTION = "audio-ms11-ext-noship Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-external-noship/audio-3rdparty/dolby/ms11/"
S = "${WORKDIR}/audio/mm-audio-external-noship/audio-3rdparty/dolby/ms11/"

ALLOW_EMPTY_${PN} = "1"
