SUMMARY = "Orb utility library"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

FILESPATH_prepend = "${WORKSPACE}/:"
SRC_URI += " \
    file://allplay/utils/ \
    file://allplay/OrbProjectFramework/ \
    "

S = "${WORKDIR}/allplay/utils/"

DEPENDS = "boost (>= 1.49) alljoyn curl libnl json-c"

inherit allplay_scons pkgconfig

EXTRA_OESCONS += "ENABLE_NETWORK_MONITOR=1"
# fix for GCC 6.0 Compilers
CXXFLAGS += "-std=gnu++98"

do_install() {
    install -d ${D}${libdir}
    install -m 0644 ${S}/build/liborbutils.a ${D}${libdir}

    install -d ${D}${includedir}/utils
    install -m 0644 ${S}/*.h ${D}${includedir}/utils

    install -d ${D}${includedir}/utils/curl
    install -m 0644 ${S}/curl/*.h ${D}${includedir}/utils/curl

    install -d ${D}${includedir}/utils/HttpServer
    install -m 0644 ${S}/HttpServer/*.h ${D}${includedir}/utils/HttpServer

    install -d ${D}${includedir}/utils/NetworkMonitor
    install -m 0644 ${S}/NetworkMonitor/*.h ${D}${includedir}/utils/NetworkMonitor

    install -d ${D}${libdir}/pkgconfig
    install -m 0644 ${S}/build/orbutils.pc ${D}${libdir}/pkgconfig/
}
