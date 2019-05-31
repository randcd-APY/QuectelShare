SUMMARY = "AllPlay media player"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

PR = "r2"

FILESPATH_prepend = "${WORKSPACE}/:"
SRC_URI = "\
           file://allplay/orbPlayer/ \
           file://allplay/tinyxml/ \
           file://allplay/OrbProjectFramework/ \
           file://50-orbplayer-input \
           file://allplay.json \
           file://orbplayer.service \
           file://HttpServer/device.crt \
           file://HttpServer/device.csr \
           file://HttpServer/device.key \
           file://HttpServer/rootCA.key \
           file://HttpServer/rootCA.pem"

SRC_DIR = "${WORKSPACE}/allplay/orbPlayer/"
S = "${WORKDIR}/allplay/orbPlayer/"

DEPENDS = "boost (>= 1.49) alljoyn alljoyn-services curl ffmpeg mdnsresponder libnl aprt audiohal soxr lms"
SDEPENDS = "orbutils spotify"
RDEPENDS_${PN} += "alljoyn alljoyn-services curl ffmpeg mdnsresponder libnl aprt allplay-base json-c lms"

inherit qprebuilt_allplay allplay_scons pkgconfig systemd

CXXFLAGS += "-std=gnu++98"

EXTRA_OESCONS += "PLAYER=IMX AUDIO_ONLY=1 HAVE_APRT=1 ENABLE_NETWORK_MONITOR=1 SPOTIFY_ENABLED=1 ENABLE_1402_DISCOVERY=y"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/build/orbPlayer ${D}${bindir}

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/orbplayer.service -D ${D}${systemd_system_unitdir}

    install -d ${D}${sysconfdir}/ssl/private/
    install -m 0644 ${WORKDIR}/HttpServer/device.crt ${D}${sysconfdir}/ssl/private/
    install -m 0644 ${WORKDIR}/HttpServer/device.key ${D}${sysconfdir}/ssl/private/

    install -d ${D}${sysconfdir}/ssl/certs/
    install -m 0644 ${WORKDIR}/HttpServer/rootCA.pem ${D}${sysconfdir}/ssl/certs/
    ln -sfn rootCA.pem ${D}${sysconfdir}/ssl/certs/`openssl x509 -noout -hash -in ${WORKDIR}/HttpServer/rootCA.pem`.0

    install -d ${D}${sysconfdir}/allplay/
    install -m 0644 ${WORKDIR}/allplay.json ${D}${sysconfdir}/allplay/

    install -d ${D}${sysconfdir}/hotplug.d/input
    install -m 0644 ${WORKDIR}/50-orbplayer-input ${D}${sysconfdir}/hotplug.d/input/

    install -d ${D}/etc/spotify
    install -m 0644 ${WORKDIR}/allplay/orbPlayer/resources/spotify_key.bin ${D}/etc/spotify/
}

FILES_${PN} += " \
    ${systemd_system_unitdir} \
    ${sysconfdir}/allplay/ \
    "

SYSTEMD_SERVICE_${PN} = "orbplayer.service"
