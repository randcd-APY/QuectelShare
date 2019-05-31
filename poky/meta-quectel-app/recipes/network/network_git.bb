inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Alpha system"
PR = "r7"

DEPENDS = "diag dsutils glib-2.0 qmi qmi-framework ql-syslog ql-common-api data ql-manager"

SRC_DIR = "${WORKSPACE}/quectel-app/network/"

S = "${WORKDIR}/quectel-app/network"

ProjectName = "${QUECTEL_PROJECT_NAME}"
ProjectRev =  "${QUECTEL_PROJECT_REV}"

do_package_qa(){
}
