inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Alpha system"
PR = "r7"

DEPENDS = "ql-power"

SRC_DIR = "${WORKSPACE}/quectel-app/peripheral/power/"

S = "${WORKDIR}/quectel-app/peripheral/power"

ProjectName = "${QUECTEL_PROJECT_NAME}"
ProjectRev =  "${QUECTEL_PROJECT_REV}"

