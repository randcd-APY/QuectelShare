#
# 
# Yocto Project Development Manual.
#

SUMMARY = "audiotest application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"


S = "${WORKDIR}/quectel-core/ql-audiotest"
SRC_DIR = "${WORKSPACE}/quectel-core/ql-audiotest"

FILES_${PN} += "/usr/*"

do_compile() {
}

do_install() {
		 install -d ${D}${bindir}
	     install -m 0755 ${SRC_DIR}/usr/bin/hal_play_test -D ${D}${bindir}
		 install -m 0755 ${SRC_DIR}/usr/bin/hal_rec_test -D ${D}${bindir}
}

do_package_qa() {
}