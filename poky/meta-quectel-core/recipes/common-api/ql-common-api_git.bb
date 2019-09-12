inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Api system"
PR = "r7"

DEPENDS = "diag dsutils glib-2.0 qmi qmi-framework ql-syslog"

SRC_DIR = "${WORKSPACE}/quectel-core/ql-common-api/"

S = "${WORKDIR}/quectel-core/ql-common-api"

PACKAGES = "${PN}"
FILES_${PN} += "${libdir}/*"
FILES_${PN}-dbg += "${libdir}/.debug/*"
FILES_${PN} += "${includedir}/*"
INSANE_SKIP_${PN} = "dev-so"

FILES_SOLIBSDEV = ""

BBCLASSEXTEND = "native nativesdk"


do_install() {
    install -d ${D}${libdir}
    install -m 0755 lib/* ${D}${libdir}

    install -d ${D}${includedir}/ql_common_api
    install -m 0644 ${WORKSPACE}/quectel-core/ql-common-api/include/*.h ${D}${includedir}/ql_common_api
}

do_compile() {
}

do_package_qa() {
}
