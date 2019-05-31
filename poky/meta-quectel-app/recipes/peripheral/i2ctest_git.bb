inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Alpha system"
PR = "r7"

DEPENDS = "ql-i2c"

SRC_DIR = "${WORKSPACE}/quectel-app/peripheral/i2c/"

S = "${WORKDIR}/quectel-app/peripheral/i2c"

ProjectName = "${QUECTEL_PROJECT_NAME}"
ProjectRev =  "${QUECTEL_PROJECT_REV}"

