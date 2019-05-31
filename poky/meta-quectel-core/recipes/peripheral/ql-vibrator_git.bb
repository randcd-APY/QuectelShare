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
}
