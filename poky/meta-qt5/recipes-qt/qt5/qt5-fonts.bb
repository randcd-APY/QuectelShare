#
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

SUMMARY = "Add fonts/*ttf to /usr/lib/qt5/fonts"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://vendor/qcom/proprietary/fonts/DroidSansFallback.ttf"

S = "${WORKDIR}"
SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/fonts"

FILES_${PN} += "/usr/lib/*"

do_install_append() {
    install -d ${D}${libdir}/qt5/fonts
    install -m 0755 ${WORKDIR}/vendor/qcom/proprietary/fonts/* -D ${D}${libdir}/qt5/fonts
    ln -sf ${libdir}/qt5/fonts ${D}${libdir}/fonts
}
