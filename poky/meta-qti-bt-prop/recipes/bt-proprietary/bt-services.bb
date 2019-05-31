inherit qcommon qlicense qprebuilt

DESCRIPTION = "Bluetooth services required for MDM platforms"

PR = "r1"

SRC_DIR = "${WORKSPACE}/bt-proprietary/bt-services/"
S = "${WORKDIR}/bt-proprietary/bt-services/"

#INITSCRIPT_NAME = "bluetooth-5"
#INITSCRIPT_PARAMS = "start 99 2 3 4 5 . stop 10 0 1 6 ."

do_install() {
    # install bluetooth sysv init script
  if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
     install -m 0755 ${S}/bluetooth-5.init -D ${D}${sysconfdir}/initscripts/bluetooth-5
  else
     install -m 0755 ${S}/bluetooth-5.init -D ${D}${sysconfdir}/init.d/bluetooth-5
  fi
}
