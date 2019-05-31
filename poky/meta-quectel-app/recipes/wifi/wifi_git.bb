inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel WIFI system"
PR = "r7"

DEPENDS = "ql-wifi"

SRC_DIR = "${WORKSPACE}/quectel-app/wifi/"

S = "${WORKDIR}/quectel-app/wifi"

do_prebuild_install() {
}
