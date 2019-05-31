inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Api system"
PR = "r7"

DEPENDS = "diag dsutils glib-2.0 qmi qmi-framework ql-syslog"

SRC_DIR = "${WORKSPACE}/quectel-core/ql-common-api/"

S = "${WORKDIR}/quectel-core/ql-common-api"

FILES_${PN} += "${libdir}/" 

FILES_SOLIBSDEV = ""


do_install() {
    install -d ${D}${libdir}
    install -m 0755 lib/* ${D}${libdir}
}

do_compile() {
}

