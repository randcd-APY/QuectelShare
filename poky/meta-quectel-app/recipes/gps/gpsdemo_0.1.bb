#
# 2019/4/22	tommy.zhang 
# recipe for quec_launcher
# Yocto Project Development Manual.
#

SUMMARY = "quec_launcher application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "ql-gnss ql-common-api ql-mcm-api ql-manager"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-app/qt/gps"

S = "${WORKDIR}/quectel-app/qt/gps"
SRC_DIR = "${WORKSPACE}/quectel-app/qt/gps"

require recipes-qt/qt5/qt5.inc
DEPENDS += "qtbase"


do_install() {
		 install -d ${D}${bindir}
	     install -m 0755 GpsDemo ${D}${bindir}
}
