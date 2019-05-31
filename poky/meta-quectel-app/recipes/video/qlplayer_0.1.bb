#
# 2019/4/22	tommy.zhang 
# recipe for quec_launcher
# Yocto Project Development Manual.
#

SUMMARY = "qlplayer application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-app/qt/video/qlplayer"

S = "${WORKDIR}/quectel-app/qt/video/qlplayer"
SRC_DIR = "${WORKSPACE}/quectel-app/qt/video/qlplayer"

require recipes-qt/qt5/qt5.inc
DEPENDS += "qtbase"
DEPENDS += "media"
DEPENDS += "ffmpeg"
DEPENDS += "ql-omx-video"
DEPENDS += "adreno"

RDEPENDS_${PN} += "ql-omx-video"


FILES_${PN} += "/lib/systemd/*"

do_install() {
	 install -d ${D}${bindir}
	 install -m 0755 qlplayer ${D}${bindir}
}
