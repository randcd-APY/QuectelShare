inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Alpha system"
PR = "r7"

DEPENDS = "ql-adc"

SRC_DIR = "${WORKSPACE}/quectel-app/peripheral/adc/"

S = "${WORKDIR}/quectel-app/peripheral/adc"

ProjectName = "${QUECTEL_PROJECT_NAME}"
ProjectRev =  "${QUECTEL_PROJECT_REV}"

