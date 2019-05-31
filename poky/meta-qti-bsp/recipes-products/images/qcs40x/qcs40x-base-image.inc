inherit dm-verity
#compile abl
RDEPENDS  += "${@bb.utils.contains('TARGET_ARCH', 'arm','lib64-edk2','edk2',d)}"

# Packages to be included in rootfs creation.
IMAGE_INSTALL += "packagegroup-core-boot"
IMAGE_INSTALL += "${CORE_IMAGE_EXTRA_INSTALL}"
IMAGE_INSTALL += "start-scripts-misc-daemon"
IMAGE_INSTALL += "chrony"

IMAGE_INSTALL += "base-files"
IMAGE_INSTALL += "base-passwd"
IMAGE_INSTALL += "glib-2.0"
IMAGE_INSTALL += "libcutils"
IMAGE_INSTALL += "liblog"
IMAGE_INSTALL += "libxml2"
IMAGE_INSTALL += "system-core-adbd"
IMAGE_INSTALL += "system-core-logd"
IMAGE_INSTALL += "system-core-leprop"
IMAGE_INSTALL += "system-core-usb"
IMAGE_INSTALL += "system-core-post-boot"
IMAGE_INSTALL += "system-conf"
IMAGE_INSTALL += "system-prop"
IMAGE_INSTALL += "systemd-machine-units"
IMAGE_INSTALL += "screen"
IMAGE_INSTALL += "wireless-tools"
IMAGE_INSTALL +="${@bb.utils.contains('DISTRO_FEATURES', 'nand-boot', 'mtd-utils-ubifs', '', d)}"

# The MSM Linux minimal boot image files.
#IMAGE_INSTALL += "gdbserver"
#IMAGE_INSTALL += "gdb"
IMAGE_INSTALL += "iw"
IMAGE_INSTALL += "kernel-modules"
IMAGE_INSTALL += "libnl"
IMAGE_INSTALL += "libstdc++"
IMAGE_INSTALL += "openssl"
#IMAGE_INSTALL += "perl"
IMAGE_INSTALL += "powerapp"
IMAGE_INSTALL += "powerapp-powerconfig"
IMAGE_INSTALL += "powerapp-reboot"
IMAGE_INSTALL += "powerapp-shutdown"
IMAGE_INSTALL += "start-scripts-firmware-links"
IMAGE_INSTALL += "systemd-coredump"
IMAGE_INSTALL += "coreutils"
IMAGE_INSTALL += "kptr-restrict"

IMAGE_EXT4_SELINUX_OPTIONS = "${@bb.utils.contains('DISTRO_FEATURES','selinux',' -S ${SELINUX_FILE_CONTEXTS} ','',d)}"

IMAGE_INSTALL += "${@bb.utils.contains('DISTRO_FEATURES', 'ab-boot-support', 'start-script-ab-slot-util', '', d)}"
IMAGE_INSTALL += "${@bb.utils.contains('DISTRO_FEATURES', 'ab-boot-support', 'abctl', '', d)}"
DEPENDS += "squashfs-tools-native"

# Install recovery and its libs in system image if A/B boot is supported
IMAGE_INSTALL += "${@bb.utils.contains('DISTRO_FEATURES', 'ab-boot-support', 'recovery-bin', '', d)}"
IMAGE_INSTALL += "${@bb.utils.contains('DISTRO_FEATURES', 'ab-boot-support', 'recovery', '', d)}"

do_makesystem() {
    if ${@bb.utils.contains('DISTRO_FEATURES','nand-boot','false','true',d)}; then
     cp ${THISDIR}/${BASEMACHINE}/qcs40x-fsconfig.conf ${WORKDIR}/rootfs-fsconfig.conf
     make_ext4fs -C ${WORKDIR}/rootfs-fsconfig.conf -B ${DEPLOY_DIR_IMAGE}/system.map -s -a / ${IMAGE_EXT4_SELINUX_OPTIONS} -l ${SYSTEM_SIZE_EXT4} ${DEPLOY_DIR_IMAGE}/${SYSTEMIMAGE_TARGET} ${IMAGE_ROOTFS}
     make_ext4fs -s -a /systemrw ${IMAGE_EXT4_SELINUX_OPTIONS} -b 4096 -l ${SYSTEMRW_SIZE_EXT4} ${DEPLOY_DIR_IMAGE}/${SYSTEMRWIMAGE_TARGET}
    fi
}
