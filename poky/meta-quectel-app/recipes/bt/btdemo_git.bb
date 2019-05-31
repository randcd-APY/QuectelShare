SUMMARY = "btdemo application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "ql-bt"


FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-app/qt/bt"

S = "${WORKDIR}/quectel-app/qt/bt"
SRC_DIR = "${WORKSPACE}/quectel-app/qt/bt"

require recipes-qt/qt5/qt5.inc
DEPENDS += "qtbase"


do_install() {
	install -d ${D}${bindir}
	install -m 0755 btdemo ${D}${bindir}
}

