# Override default init script run order. This has to run after other
# initscripts have finished remounting /dev.

INITSCRIPT_PARAMS = "start 04 S ."
SELINUX_SCRIPT_DST = "${BPN}"

do_install_append() {
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'false', 'true', d)}; then
        # Sysvinit targets use inittab to create /dev/socket. The sysfs
        #  initscript will then remount /dev and that file will be lost. When
        #  the file is later recreated, it will inherit the incorrect parent
        #  context of device_t. Recreate the directory before relabeling.
        sed -i '/\${RESTORECON}/i \
mkdir -p \/dev\/socket -m 777' ${D}${sysconfdir}/init.d/selinux-labeldev
    else
        sed -i '/\${RESTORECON}/i \
mkdir -p \/dev\/socket -m 777' ${D}${bindir}/selinux-labeldev.sh
    fi
}
