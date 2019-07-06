inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel WIFI system"
PR = "r7"

DEPENDS = "mcm-core ql-mcm-api"

SRC_DIR = "${WORKSPACE}/quectel-app/mcm-api-test/"

S = "${WORKDIR}/quectel-app/mcm-api-test"

do_prebuild_install() {
}
