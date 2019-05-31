inherit update-rc.d qcommon qlicense systemd

DESCRIPTION = "Start up script for le-updater"

DEPENDS += "abctl"

SRC_URI = "file://start_le-updater"
SRC_URI += "file://le-updater"
SRC_URI += "file://le-updater_daemon.service"

PR = "r2"

INITSCRIPT_NAME = "start-le-updater"
INITSCRIPT_PARAMS = "start 90 2 3 4 5 . stop 10 0 1 6 ."

do_compile[noexec] = "1"

do_install() {
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -m 0750 ${WORKDIR}/start_le-updater -D ${D}${sysconfdir}/initscripts/${INITSCRIPT_NAME}
        install -m 0754 ${WORKDIR}/le-updater -D ${D}${bindir}/le-updater
        install -d ${D}/etc/systemd/system/
        install -m 0644 ${WORKDIR}/le-updater_daemon.service -D ${D}/etc/systemd/system/le-updater_daemon.service
        install -d ${D}/etc/systemd/system/multi-user.target.wants/
        # enable the service for multi-user.target
        ln -sf /etc/systemd/system/le-updater_daemon.service \
                 ${D}/etc/systemd/system/multi-user.target.wants/le-updater_daemon.service
    else
        install -m 0754 ${WORKDIR}/start_le-updater -D ${D}${sysconfdir}/init.d/${INITSCRIPT_NAME}
        install -m 0754 ${WORKDIR}/le-updater -D ${D}${bindir}/le-updater
    fi
}

FILES_${PN} += "${systemd_unitdir}/system/"
