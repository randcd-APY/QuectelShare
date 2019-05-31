SUMMARY = "APRT library"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

FILESPATH_prepend = "${WORKSPACE}/:"
SRC_URI = "\
           file://allplay/aprt/ \
           file://allplay/OrbProjectFramework/ \
           "

SRC_DIR = "${WORKSPACE}/allplay/aprt/"
S = "${WORKDIR}/allplay/aprt/"

DEPENDS = "boost (>= 1.49) usrsctp"
SDEPENDS = "orbutils"
RDEPENDS_${PN} += "usrsctp"

inherit qprebuilt_allplay allplay_scons pkgconfig

CXXFLAGS += "-std=c++98"

do_install() {
    install -d ${D}${includedir}/aprt
    install -m 0644 ${S}/include/aprt/*.h ${D}${includedir}/aprt

    install -d ${D}${libdir}
    install -m 0755 ${S}/build/libaprt.so ${D}${libdir}

    install -d ${D}${libdir}/pkgconfig
    install -m 0644 ${S}/build/aprt.pc ${D}${libdir}/pkgconfig/
}

FILES_SOLIBSDEV = ""

FILES_${PN} += "${libdir}/libaprt.so"
