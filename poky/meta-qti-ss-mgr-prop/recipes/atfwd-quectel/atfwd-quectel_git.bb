inherit qcommon qlicense qprebuilt

DESCRIPTION = "Daemon to handle AT quectel commands"

DEPENDS = "glib-2.0 qmi qmi-framework"

SRC_DIR = "${WORKSPACE}/mdm-ss-mgr/atfwd-quectel/"
S = "${WORKDIR}/mdm-ss-mgr/atfwd-quectel/"

PR = "r3"
FILES_${PN} += "/lib/systemd/*"
do_install_append() {
   if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
      install -d ${D}/etc/initscripts
	  install -d ${D}${systemd_unitdir}/system
     mv ${D}/etc/init.d/start_atfwd_daemon_quec ${D}/etc/initscripts/start_atfwd_daemon_quec
	 install -m 0644 ${WORKDIR}/mdm-ss-mgr/atfwd-quectel/atfwd_quectel.service ${D}${systemd_unitdir}/system/atfwd_quectel.service
	 install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
	 ln -sf ${systemd_unitdir}/system/atfwd_quectel.service  ${D}${systemd_unitdir}/system/multi-user.target.wants/atfwd_quectel.service
  fi
}
