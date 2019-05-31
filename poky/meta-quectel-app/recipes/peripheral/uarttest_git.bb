inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Alpha system"
PR = "r7"

DEPENDS = "ql-uart"

SRC_DIR = "${WORKSPACE}/quectel-app/peripheral/uart/"

S = "${WORKDIR}/quectel-app/peripheral/uart"

ProjectName = "${QUECTEL_PROJECT_NAME}"
ProjectRev =  "${QUECTEL_PROJECT_REV}"

