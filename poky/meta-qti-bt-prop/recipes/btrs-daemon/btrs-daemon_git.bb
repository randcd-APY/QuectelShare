inherit qcommon qlicense qprebuilt

DESCRIPTION = "Bluetooth Remote Start DAEMON"
PR = "r2"
DEPENDS = "data"

SRC_DIR = "${WORKSPACE}/bt-proprietary/btrs-daemon"
S = "${WORKDIR}/btrs-daemon"

do_install_append() {
        install -m 0755 ${WORKDIR}/btrs-daemon/start_btrsdaemon -D ${D}${sysconfdir}/init.d/btrsdaemon
}
