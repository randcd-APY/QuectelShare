inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Alpha system"
PR = "r7"

DEPENDS = "ql-gpio"

SRC_DIR = "${WORKSPACE}/quectel-app/peripheral/gpio/"

S = "${WORKDIR}/quectel-app/peripheral/gpio"

ProjectName = "${QUECTEL_PROJECT_NAME}"
ProjectRev =  "${QUECTEL_PROJECT_REV}"
