#
# 2019/8/29	javed.wu 
# recipe for quec_launcher
# Yocto Project Development Manual.
#

SUMMARY = "quec_launcher application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://qtmultimedia"

S = "${WORKDIR}/qtmultimedia"
SRC_DIR = "${WORKSPACE}/qtmultimedia"

require recipes-qt/qt5/qt5.inc

DEPENDS += "qtbase"
DEPENDS += "qtdeclarative"
DEPENDS += "gstreamer1.0"
DEPENDS += "gstreamer1.0-plugins-base"
DEPENDS += "gstreamer1.0-plugins-good"
DEPENDS += "gstreamer1.0-plugins-bad"
DEPENDS += "gstreamer1.0-libav"
DEPENDS += "gstreamer1.0-omx"
DEPENDS += "alsa-lib"
DEPENDS += "alsa-utils"
DEPENDS += "alsa-tools"

FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${libdir}/*.so.*.*.* ${sysconfdir}/* ${bindir}/* ${libdir}/pkgconfig/* ${base_prefix}/*"
FILES_${PN}-dev  += "${libdir}/*.la ${includedir}"
FILES_${PN} += "/usr/bin/*"

inherit qmake5

do_install() {
	install -d ${D}${includedir}/qt5
	install -d ${D}${libdir}/qt5/qml/QtMultimedia
	install -d ${D}${bindir}
	cp -f lib/libQt5Multimedia.so* ${D}${libdir}/qt5/qml/QtMultimedia/
	cp -f lib/libQt5MultimediaQuick_p.so* ${D}${libdir}/qt5/qml/QtMultimedia/
	cp -f examples/multimedia/video/qmlvideo/qmlvideo ${D}${bindir}/
	cp -f qml/QtMultimedia/libdeclarative_multimedia.so ${D}${libdir}/qt5/qml/QtMultimedia
	cp -f qml/QtMultimedia/qmldir ${D}${libdir}/qt5/qml/QtMultimedia
}
