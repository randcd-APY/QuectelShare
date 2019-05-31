inherit qcommon qlicense systemd update-rc.d
DESCRIPTION = "Start up script for misc daemons"

SRC_URI ="file://start_misc-daemon"
SRC_URI +="file://misc_daemon.service"
S = "${WORKDIR}"
SRC_DIR = "${THISDIR}"

PR = "r2"

INITSCRIPT_NAME = "misc-daemon"
INITSCRIPT_PARAMS = "start 98 5 . stop 2 0 1 6 ."

do_compile[noexec] = "1"

do_install() {
       if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -m 0750 ${WORKDIR}/start_misc-daemon -D ${D}${sysconfdir}/initscripts/${INITSCRIPT_NAME}
        install -d ${D}/etc/systemd/system/
        install -m 0644 ${WORKDIR}/misc_daemon.service -D ${D}/etc/systemd/system/misc_daemon.service
        install -d ${D}/etc/systemd/system/multi-user.target.wants/
        # enable the service for multi-user.target
        ln -sf /etc/systemd/system/misc_daemon.service \
              ${D}/etc/systemd/system/multi-user.target.wants/misc_daemon.service
       else
        install -m 0755 ${WORKDIR}/start_misc-daemon -D ${D}${sysconfdir}/init.d/${INITSCRIPT_NAME}
       fi
}

FILES_${PN} += "${systemd_unitdir}/system/" 
