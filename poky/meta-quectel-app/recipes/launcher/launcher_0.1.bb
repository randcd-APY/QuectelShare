#
# 2019/4/22	tommy.zhang 
# recipe for quec_launcher
# Yocto Project Development Manual.
#

SUMMARY = "quec_launcher application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-app/qt/quec_launcher"

S = "${WORKDIR}/quectel-app/qt/quec_launcher"
SRC_DIR = "${WORKSPACE}/quectel-app/qt/quec_launcher"

require recipes-qt/qt5/qt5.inc
DEPENDS += "qtbase"

FILES_${PN} += "/lib/systemd/*"
FILES_${PN} += "/etc/systemd/*"

do_install() {
		 install -d ${D}${bindir}
	     install -m 0755 quec_launcher ${D}${bindir}

         #added by peeta
         install -d ${D}${systemd_unitdir}/system/
         install -m 0644 ${WORKDIR}/quectel-app/qt/quec_launcher/quec_launcher.service.in \
         ${D}${systemd_unitdir}/system/quec_launcher.service

         install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants/

         ln -sf ${systemd_unitdir}/system/quec_launcher.service \
         ${D}${sysconfdir}/systemd/system/multi-user.target.wants/quec_launcher.service
}
