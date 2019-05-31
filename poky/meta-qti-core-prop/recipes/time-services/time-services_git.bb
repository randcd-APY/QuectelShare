inherit autotools qcommon qlicense update-rc.d qprebuilt systemd
DESCRIPTION = "Time Services Daemon"
PR = "r9"

SRC_DIR = "${WORKSPACE}/time-services"
S = "${WORKDIR}/time-services"

DEPENDS += "virtual/kernel"
DEPENDS += "glib-2.0"
DEPENDS += "diag"
DEPENDS += "qmi-framework"
RDEPENDS_${PN} += "qmi-framework"


INITSCRIPT_NAME = "time_serviced"
INITSCRIPT_PARAMS = "start 29 2 3 4 5 . stop 1 0 1 6 ."

SRC_URI +="file://time_serviced.service"
CFLAGS += "-lpthread"
do_install_append () {

    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
     install -m 0755 ${WORKDIR}/time-services/time_serviced -D ${D}${sysconfdir}/initscripts/time_serviced
     install -d ${D}/etc/systemd/system/
     install -m 0644 ${WORKDIR}/time_serviced.service -D ${D}/etc/systemd/system/time_serviced.service
     install -d ${D}/etc/systemd/system/multi-user.target.wants/
     ln -sf /etc/systemd/system/time_serviced \
			              ${D}/etc/systemd/system/multi-user.target.wants/time_serviced.service
 
    else
      install -m 0755 ${S}/time_serviced -D ${D}${sysconfdir}/init.d/time_serviced
    fi
}

FILES_${PN} += "${systemd_unitdir}/system/"
