SUMMARY = "quectel uart lib"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-core/ql-peripheral/ql-uart"

SRC_DIR = "${WORKSPACE}/quectel-core/ql-peripheral/ql-uart"
S = "${WORKDIR}/quectel-core/ql-peripheral/ql-uart"

FILES_${PN} += "${libdir}/" 

FILES_SOLIBSDEV = ""

do_install() {
    install -d ${D}${libdir}
    install -m 0755 lib/* ${D}${libdir}
    ln -sf libql_uart.so ${D}${libdir}/libql_uart.so.0

    install -d ${D}${includedir}/ql_uart
    install -m 0644 ${WORKSPACE}/quectel-core/ql-peripheral/ql-uart/include/ql_uart.h ${D}${includedir}/ql_uart
}

do_compile() {
}

do_package_qa() {
}

PACKAGES = "${PN}"
FILES_${PN} += "${libdir}/*"
FILES_${PN}-dbg += "${libdir}/.debug/*"
FILES_${PN} += "${includedir}/*"
INSANE_SKIP_${PN} = "dev-so"

BBCLASSEXTEND = "native nativesdk"
