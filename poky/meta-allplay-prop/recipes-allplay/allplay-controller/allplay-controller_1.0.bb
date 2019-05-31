SUMMARY = "AllPlay Controller"
LICENSE = "CLOSED"
PR = "r0"

FILESPATH_prepend = "${WORKSPACE}/:"
SRC_URI = " \
    file://allplay/PlayTo/ \
    file://allplay/OrbProjectFramework/ \
    "
S = "${WORKDIR}/allplay/PlayTo/orbplaycli/"
SRC_DIR = "${WORKSPACE}/allplay/PlayTo/orbplaycli/"

DEPENDS = "boost (>= 1.49) alljoyn alljoyn-services aprt"
SDEPENDS = "orbutils"
RDEPENDS_${PN} += "alljoyn alljoyn-services aprt"

inherit qprebuilt_allplay allplay_scons pkgconfig

CXXFLAGS += "-std=gnu++98 -DCONNECT_TO_LOCAL_ONLY -DUSE_MCU_SESSION_PORT -DNDEBUG -g3"
CXXFLAGS += "-I${STAGING_INCDIR}"

EXTRA_OESCONS += "PLAYER_ONLY=1"

do_install() {
    install -d ${D}${libdir}
    install -m 0755 ${S}/build/liballplay-controller.so ${D}${libdir}
    install -m 0755 ${S}/../liborbplay/build/liborbplay.so ${D}${libdir}

    install -d ${D}${includedir}
    install -m 0755 ${S}/../liborbplay/include/liborbplay.h ${D}${includedir}

}

FILES_SOLIBSDEV = ""
SOLIBS = ".so"
