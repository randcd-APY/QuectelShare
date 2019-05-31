inherit qcommon qprebuilt qlicense

DESCRIPTION = "esp Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/esp/"
S = "${WORKDIR}/audio/mm-audio/esp/"

EXTRA_OECONF_append_msm8909 = "BOARD_SUPPORTS_ESP=false"

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
