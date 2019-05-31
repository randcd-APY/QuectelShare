inherit qcommon qlicense qprebuilt

DESCRIPTION = "audio-qaf"
SECTION = "multimedia"
PR = "r0"

DEPENDS = "glib-2.0 libcutils system-media"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/audio-qaf/"
S = "${WORKDIR}/audio/mm-audio/audio-qaf/"

EXTRA_OECONF = "--with-glib"

ALLOW_EMPTY_${PN} = "1"
