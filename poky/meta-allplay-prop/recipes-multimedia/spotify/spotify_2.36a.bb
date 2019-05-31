DESCRIPTION="Spotify 32 bit prebuilt library for arm"
SECTION = "audio"

LICENSE = "CLOSED"

SRC_DIR = "${WORKSPACE}/allplay/orbPlayer/spotify-esdk/"
FILESPATH_prepend = "${SRC_DIR}:"

SRC_URI = "file://spotify_embedded.h \
           file://libspotify_embedded_shared.so"

COMPATIBLE_HOST = "arm.*-linux"

inherit qprebuilt

# Prevent 'already stripped' warnings
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"

SOLIBS = ".so"
FILES_SOLIBSDEV = ""

RDEPENDS_${PN} = 'mdnsresponder'

do_install() {
    install -d ${D}${includedir}/spotify
    install -m 0644 ${WORKDIR}/spotify_embedded.h ${D}${includedir}/spotify/

    install -d ${D}${libdir}
    install -m 0755 ${WORKDIR}/libspotify_embedded_shared.so ${D}${libdir}
}

