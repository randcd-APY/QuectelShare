#
# Yocto Project Development Manual.
#

SUMMARY = "Simple ql-input application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

#FILESPATH =+ "${WORKSPACE}:"

S = "${WORKDIR}/quectel-core/ql-peripheral/ql-input"
SRC_DIR = "${WORKSPACE}/quectel-core/ql-peripheral/ql-input"

FILES_${PN} += "/lib/systemd/*"

do_install() {
       install -d ${D}${bindir}
       install -m 0755 ${SRC_DIR}/usr/bin/ql_input ${D}${bindir}

       install -d ${D}${systemd_unitdir}/system
	   install -m 0644 ${SRC_DIR}/lib/systemd/system/ql_input.service \
		 ${D}${systemd_unitdir}/system/ql_input.service

	install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/

    ln -sf ${systemd_unitdir}/system/ql_input.service \
		${D}${systemd_unitdir}/system/multi-user.target.wants/ql_input.service
}

do_compile() {
}

do_package_qa() {
}