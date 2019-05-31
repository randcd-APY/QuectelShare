SUMMARY = "quectel gpio lib"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-core/ql-peripheral/ql-gpio"

SRC_DIR = "${WORKSPACE}/quectel-core/ql-peripheral/ql-gpio"
S = "${WORKDIR}/quectel-core/ql-peripheral/ql-gpio"

FILES_${PN} += "${libdir}/" 

FILES_SOLIBSDEV = ""

do_install() {
		 install -d ${D}${libdir}
		 install -m 0755 lib/* ${D}${libdir}
		 ln -sf libql_gpio.so ${D}${libdir}/libql_gpio.so.0
}

do_compile() {
}
