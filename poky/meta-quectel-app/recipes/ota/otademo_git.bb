#
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

SUMMARY = "Simple ota demo"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-app/otademo/"

S = "${WORKDIR}/quectel-app/otademo"
SRC_DIR = "${WORKSPACE}/quectel-app/otademo"

inherit cmake

DEPENDS = "cmake (>= 2.6.3)"
DEPENDS += "ql-ota"
#EXTRA_OECMAKE +="-DBUILD_SHARED_LIBS=OFF"
EXTRA_OECMAKE_append += " -DCMAKE_SKIP_RPATH=ON"
EXTRA_OECMAKE_append += " -DCMAKE_INSTALL_DO_STRIP=1"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/build/util/ota-test -D ${D}${bindir}/
#chrpath -d ${D}${bindir}/debug-test
}

FILES_${PN} += "${bindir}/" 

FILES_SOLIBSDEV = ""
