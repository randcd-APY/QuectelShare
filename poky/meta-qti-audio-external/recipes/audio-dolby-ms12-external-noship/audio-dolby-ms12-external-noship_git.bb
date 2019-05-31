inherit qcommon qlicense qprebuilt

DESCRIPTION = "ms12-library"
SECTION = "multimedia"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-external-noship/audio-3rdparty/dolby/ms12/"
S = "${WORKDIR}/audio/mm-audio-external-noship/audio-3rdparty/dolby/ms12/"

EXTRA_OECONF_append_apq8098 = "MS12_SECURITY_FEATURE_ENABLED=true"
EXTRA_OECONF_append_apq8098 = " AUDIO_FEATURE_ENABLED_QAP=true"
EXTRA_OECONF_append_aarch64 = " AUDIO_FEATURE_MS12_64BIT_ENABLED=true"

ALLOW_EMPTY_${PN} = "1"
