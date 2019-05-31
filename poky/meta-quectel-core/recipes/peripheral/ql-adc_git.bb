SUMMARY = "quectel adc lib"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-core/ql-peripheral/ql-adc"

SRC_DIR = "${WORKSPACE}/quectel-core/ql-peripheral/ql-adc"
S = "${WORKDIR}/quectel-core/ql-peripheral/ql-adc"



FILES_${PN} += "${libdir}/" 

FILES_SOLIBSDEV = ""

do_install() {
		 install -d ${D}${libdir}
		 install -m 0755 ${WORKSPACE}/quectel-core/ql-peripheral/ql-adc/lib/libql_adc.so ${D}${libdir}
		 ln -sf libql_adc.so ${D}${libdir}/libql_adc.so.0
}
