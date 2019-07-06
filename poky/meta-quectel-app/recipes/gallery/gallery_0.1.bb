#
# 2019/5/14	barnett.wang 
# recipe for quec_launcher
# Yocto Project Development Manual.
#

SUMMARY = "gallery application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"


FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-app/qt/gallery"

S = "${WORKDIR}/quectel-app/qt/gallery"
SRC_DIR = "${WORKSPACE}/quectel-app/qt/gallery"

require recipes-qt/qt5/qt5.inc
DEPENDS += "qtbase"


do_install() {
		 install -d ${D}${bindir}
	     install -m 0755 Gallery ${D}${bindir}
}
