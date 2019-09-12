SUMMARY = "quectel vibrator lib"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-core/ql-peripheral/ql-vibrator"

SRC_DIR = "${WORKSPACE}/quectel-core/ql-peripheral/ql-vibrator"
S = "${WORKDIR}/quectel-core/ql-peripheral/ql-vibrator"



FILES_${PN} += "${libdir}/" 

FILES_SOLIBSDEV = ""

do_install() {
    install -d ${D}${libdir}
    install -m 0755 ${WORKSPACE}/quectel-core/ql-peripheral/ql-vibrator/lib/libql_vibrator.so ${D}${libdir}
    ln -sf libql_vibrator.so ${D}${libdir}/libql_vibrator.so.0

    install -d ${D}${includedir}/ql_vibrator
    install -m 0644 ${WORKSPACE}/quectel-core/ql-peripheral/ql-vibrator/include/ql_vibrator.h ${D}${includedir}/ql_vibrator
}

do_package_qa() {
}

PACKAGES = "${PN}"
FILES_${PN} += "${libdir}/*"
FILES_${PN}-dbg += "${libdir}/.debug/*"
FILES_${PN} += "${includedir}/*"
INSANE_SKIP_${PN} = "dev-so"

BBCLASSEXTEND = "native nativesdk"
