inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Alpha system"
PR = "r7"

DEPENDS = "ql-sensor"

SRC_DIR = "${WORKSPACE}/quectel-app/peripheral/sensor/"

S = "${WORKDIR}/quectel-app/peripheral/sensor"

ProjectName = "${QUECTEL_PROJECT_NAME}"
ProjectRev =  "${QUECTEL_PROJECT_REV}"

