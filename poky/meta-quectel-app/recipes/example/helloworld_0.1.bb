#
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

SUMMARY = "Simple helloworld application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-app/example/helloworld"

SRC_DIR = "${WORKSPACE}/quectel-app/example/helloworld"
S = "${WORKDIR}/quectel-app/example/helloworld"


do_compile() {
	     make
}

do_install() {
		 install -d ${D}${bindir}
	     install -m 0755 helloworld ${D}${bindir}
}
