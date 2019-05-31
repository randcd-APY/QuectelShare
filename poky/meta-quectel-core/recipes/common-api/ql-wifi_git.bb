SUMMARY = "quectel wifi lib"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-core/ql-wifi"

SRC_DIR = "${WORKSPACE}/quectel-core/ql-wifi"
S = "${WORKDIR}/quectel-core/ql-wifi"

DEPENDS = "wpa-supplicant-qcacld"


FILES_${PN} += "${libdir}/" 

FILES_SOLIBSDEV = ""


do_install() {
    install -d ${D}${libdir}
    install -m 0755 lib/* ${D}${libdir}
}

do_package_qa() {
}

