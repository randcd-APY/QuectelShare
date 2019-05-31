inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Alpha system"
PR = "r7"

DEPENDS = "ql-vibrator"

SRC_DIR = "${WORKSPACE}/quectel-app/peripheral/vibrator/"

S = "${WORKDIR}/quectel-app/peripheral/vibrator"

ProjectName = "${QUECTEL_PROJECT_NAME}"
ProjectRev =  "${QUECTEL_PROJECT_REV}"

