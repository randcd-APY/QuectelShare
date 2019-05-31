inherit qcommon autotools qlicense

DESCRIPTION = "Fileipc provides PLL clock dift correction mechanism"

PR = "r0"

DEPENDS = "audiohal"

SRC_URI = "file://vendor/qcom/proprietary/media-hub//fileipc/"
S = "${WORKDIR}/vendor/qcom/proprietary/media-hub/fileipc/"
