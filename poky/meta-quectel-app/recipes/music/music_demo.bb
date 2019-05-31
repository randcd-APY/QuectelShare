#
# 2019/4/22	javed.wu 
# recipe for quec_launcher
# Yocto Project Development Manual.
#

SUMMARY = "quec_launcher application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-app/qt/music"

S = "${WORKDIR}/quectel-app/qt/music"
SRC_DIR = "${WORKSPACE}/quectel-app/qt/music"

require recipes-qt/qt5/qt5.inc
DEPENDS += "qtbase audiohal ql-audio"


do_install() {
		 install -d ${D}${bindir}
	     install -m 0755 MusicDemo ${D}${bindir}
}
