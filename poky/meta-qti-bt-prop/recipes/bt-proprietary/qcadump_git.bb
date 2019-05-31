inherit autotools qcommon qlicense

DESCRIPTION = "Qualcomm Technologies Inc qcadump"

PR = "r1"

SRC_DIR = "${WORKSPACE}/bt-proprietary/qcadump/"

S = "${WORKDIR}/bt-proprietary/qcadump"

EXTRA_OECONF += "--enable-target=${BASEMACHINE}"
