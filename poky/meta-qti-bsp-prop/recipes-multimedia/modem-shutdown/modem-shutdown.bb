inherit autotools systemd update-rc.d qcommon qlicense

DESCRIPTION = "Automatically shuts the modem down on halt and reboot"
PR="r2"

SRC_URI = "file://modem-shutdown.sh"
SRC_URI += "file://modem_shutdown.service"

S = "${WORKDIR}"
SRC_DIR = "${THISDIR}"

#does not appear to work if run as start 01
INITSCRIPT_NAME = "modem-shutdown"
INITSCRIPT_PARAMS = "stop 01 0 6 ."

SSTATE_CREATE_PKG = "0"

do_compile[noexec] = "1"

do_install() {
       if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
          install -m 0750 ${S}/modem-shutdown.sh -D ${D}/${sysconfdir}/initscripts/modem-shutdown
          mkdir -p ${D}/bin
          ln -s /etc/initscripts/modem-shutdown ${D}/bin/modem-shutdown         
          install -d ${D}${systemd_unitdir}/system/
          install -m 0644 ${WORKDIR}/modem_shutdown.service -D ${D}${systemd_unitdir}/system/modem_shutdown.service
       else
          install -m 0755 ${S}/modem-shutdown.sh -D ${D}/${sysconfdir}/init.d/modem-shutdown
          mkdir -p ${D}/bin
          ln -s /etc/init.d/modem-shutdown ${D}/bin/modem-shutdown
       fi
    
}

FILES_${PN} += "${systemd_unitdir}/system/" 
