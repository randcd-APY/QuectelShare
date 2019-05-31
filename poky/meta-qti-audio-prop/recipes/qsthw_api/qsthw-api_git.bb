inherit autotools qcommon qlicense qprebuilt

DESCRIPTION = "qsthw_api Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/qsthw_api/"
S = "${WORKDIR}/audio/mm-audio/qsthw_api/"

DEPENDS = "tinyalsa libcutils libhardware soundtrigger qsthw"
DEPENDS_append_msm8909 = " qti-audio-server binder"
DEPENDS_append_apq8017 = " qti-audio-server binder"

EXTRA_OECONF += "BOARD_SUPPORTS_QSTHW_API=true"
EXTRA_OECONF_append_apq8017 = " BOARD_SUPPORTS_QTI_AUDIO_SERVER=true"
EXTRA_OECONF_append_msm8909 = " BOARD_SUPPORTS_QTI_AUDIO_SERVER=true"

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
