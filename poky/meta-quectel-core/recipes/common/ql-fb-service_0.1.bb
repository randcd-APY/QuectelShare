#
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

SUMMARY = "Simple ql-fb-service application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

#FILESPATH =+ "${WORKSPACE}:"

S = "${WORKDIR}/quectel-core/ql-fb-service"
SRC_DIR = "${WORKSPACE}/quectel-core/ql-fb-service"

FILES_${PN} += "/lib/systemd/*"
FILES_${PN} += "/usr/*"
FILES_${PN} += "/etc/systemd/*"

do_compile() {
}

do_install() {
		 install -d ${D}${bindir}
	     install -m 0755 ${SRC_DIR}/usr/bin/ql-fb-service ${D}${bindir}

         install -d ${D}${systemd_unitdir}/system
         install -m 0644 ${SRC_DIR}/lib/systemd/system/ql-fb-service.service \
         ${D}${systemd_unitdir}/system/ql-fb-service.service

         install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants

         ln -sf ${systemd_unitdir}/system/ql-fb-service.service \
         ${D}${sysconfdir}/systemd/system/multi-user.target.wants/ql-fb-service.service
}
