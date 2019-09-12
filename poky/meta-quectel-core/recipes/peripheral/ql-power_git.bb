SUMMARY = "quectel power lib"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-core/ql-peripheral/ql-power"

SRC_DIR = "${WORKSPACE}/quectel-core/ql-peripheral/ql-power"
S = "${WORKDIR}/quectel-core/ql-peripheral/ql-power"



FILES_${PN} += "${libdir}/" 

FILES_SOLIBSDEV = ""

do_install() {
    install -d ${D}${libdir}
    install -m 0755 ${WORKSPACE}/quectel-core/ql-peripheral/ql-power/lib/libql_power.so ${D}${libdir}
    ln -sf libql_power.so ${D}${libdir}/libql_power.so.0

    install -d ${D}${includedir}/ql_power
    install -m 0644 ${WORKSPACE}/quectel-core/ql-peripheral/ql-power/include/ql_power.h ${D}${includedir}/ql_power
}

do_package_qa() {
}

PACKAGES = "${PN}"
FILES_${PN} += "${libdir}/*"
FILES_${PN}-dbg += "${libdir}/.debug/*"
FILES_${PN} += "${includedir}/*"
INSANE_SKIP_${PN} = "dev-so"

BBCLASSEXTEND = "native nativesdk"
