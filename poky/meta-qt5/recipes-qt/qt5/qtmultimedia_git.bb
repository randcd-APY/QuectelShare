require qt5.inc
require qt5-ptest.inc

SUMMARY = "qtmultimeida application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://qtmultimedia"

S = "${WORKDIR}/qtmultimedia"
SRC_DIR = "${WORKSPACE}/qtmultimedia"

DEPENDS += "qtbase"
DEPENDS += "qtdeclarative"
DEPENDS += "gstreamer1.0"
DEPENDS += "gstreamer1.0-plugins-base"
DEPENDS += "gstreamer1.0-plugins-good"
DEPENDS += "gstreamer1.0-plugins-bad"
DEPENDS += "gstreamer1.0-libav"
DEPENDS += "alsa-lib"
DEPENDS += "alsa-utils"
DEPENDS += "alsa-tools"

FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${libdir}/*.so.*.*.* ${sysconfdir}/* ${bindir}/* ${libdir}/pkgconfig/* ${base_prefix}/*"
FILES_${PN}-dev  += "${libdir}/*.la ${includedir}"
do_install() {
	install -d ${D}${includedir}/qt5
	install -d ${D}${libdir}/qt5
	install -d ${D}${bindir}
	cp -rf lib/* ${D}${libdir}
	cp -rf plugins ${D}${libdir}/qt5/
	cp -rf include/* ${D}${includedir}/qt5/
	cp -f examples/multimediawidgets/player/player ${D}${bindir}/
}
