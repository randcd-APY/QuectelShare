inherit autotools qcommon qlicense

DESCRIPTION = "pulse audio qti soundtrigger Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/pulseaudio/qti_soundtrigger/"
S = "${WORKDIR}/audio/mm-audio/pulseaudio/qti_soundtrigger/"

DEPENDS = "pulseaudio glib-2.0"

EXTRA_OECONF = "--with-glib"

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
