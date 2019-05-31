#
# 2019/5/14	barnett.wang 
# recipe for quec_launcher
# Yocto Project Development Manual.
#

SUMMARY = "camerademo application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS += "libRoboticsCamera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-app/qt/camerademo"

S = "${WORKDIR}/quectel-app/qt/camerademo"
SRC_DIR = "${WORKSPACE}/quectel-app/qt/camerademo"

require recipes-qt/qt5/qt5.inc
DEPENDS += "qtbase"
DEPENDS += "ffmpeg"
DEPENDS += "ql-omx-video"

RDEPENDS_${PN} += "ql-omx-video"


do_install() {
		 install -d ${D}${bindir}
	     install -m 0755 camerademo ${D}${bindir}
}
