SUMMARY = "AllPlay controller client"
LICENSE = "CLOSED"

PR = "r1"

FILESPATH_prepend = "${WORKSPACE}/:"
SRC_URI = "file://allplay/liballplaymcu/"

SRC_DIR = "${WORKSPACE}/allplay/liballplaymcu/"
S = "${WORKDIR}/allplay/liballplaymcu/"

DEPENDS = "alljoyn-thinclient"
RDEPENDS_${PN} = "alljoyn-thinclient"

inherit qprebuilt_allplay allplay_scons

CXXFLAGS += "-DDISABLE_MCU_VERSION"

VARIANT = "release"
EXTRA_OESCONS += "TARG=le VARIANT=${VARIANT}"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/build/${VARIANT}/allplay-cli ${D}${bindir}
}
