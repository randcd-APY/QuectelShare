inherit qcommon qlicense qprebuilt

DESCRIPTION = "Daemon to handle AT commands"

DEPENDS = "glib-2.0 qmi qmi-framework"

SRC_DIR = "${WORKSPACE}/mdm-ss-mgr/atfwd-daemon/"
S = "${WORKDIR}/mdm-ss-mgr/atfwd-daemon/"

PR = "r3"

do_install_append() {
  if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
     install -d ${D}/etc/initscripts
     mv ${D}/etc/init.d/start_atfwd_daemon ${D}/etc/initscripts/start_atfwd_daemon
  fi
}
