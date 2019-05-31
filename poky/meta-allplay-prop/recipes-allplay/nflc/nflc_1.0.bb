SUMMARY = "nflc DLNA"
LICENSE = "CLOSED"
PR = "r0"
FILESPATH_prepend = "${WORKSPACE}/:"

SRC_URI = "file://allplay/nflc/ \
           file://nflc.service \
          "

SRC_DIR = "${WORKSPACE}/allplay/nflc/"
S = "${WORKDIR}/allplay/nflc"

DEPENDS = "expat allplay-controller curl"
RDEPENDS_${PN} = "allplay-controller"

#TBD OE_BUILD, temporary variable to mask out openwrt specific variables e.g toolchain
EXTRA_OEMAKE = "MKDIR='mkdir' CP='cp -f' RM='rm' LDFLAGS='-lpthread -ldl' OE_BUILD=1"

CLEANBROKEN = "1"

inherit qprebuilt systemd

do_install() {
    install -d ${D}${datadir}/dlna/xml ${D}${datadir}/dlna/xml/icon
    install -m 0644 ${S}/data/*.xml ${D}${datadir}/dlna/xml
    install -m 0644 ${S}/data/icon/* ${D}${datadir}/dlna/xml/icon

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/nflc.service -D ${D}${systemd_system_unitdir}

    install -d ${D}${bindir}
    install -m 0755 ${S}/nfdlna ${D}${bindir}

    install -d ${D}${libdir}
    install -m 0644 ${S}/libs/src/dlna/target/linux/libs/libdmr.a ${D}${libdir}
    install -m 0644 ${S}/libs/src/dlna/target/linux/libs/libupnp_server_newdmr.a ${D}${libdir}
    install -m 0644 ${S}/libs/src/dlna/target/linux/libs/libdlnamedia.a ${D}${libdir}
    install -m 0644 ${S}/libs/src/dlna/target/linux/libs/libdlnautil.a ${D}${libdir}
    install -m 0644 ${S}/libs/src/dlna/target/linux/libs/libdlnacommon.a ${D}${libdir}
    install -m 0644 ${S}/libs/src/dlna/target/linux/libs/ext/libinet.a ${D}${libdir}
    install -m 0644 ${S}/libs/src/dlna/target/linux/libs/ext/libslim.a ${D}${libdir}
    install -m 0644 ${S}/libs/src/dlna/target/linux/libs/ext/libslim_peer.a ${D}${libdir}

}

FILES_${PN} += " \
	${datadir}/dlna/ \
	${systemd_system_unitdir} \
	"

SYSTEMD_SERVICE_${PN} = "nflc.service"
