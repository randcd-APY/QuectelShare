# Rootfs creation.

DEPENDS += "lk"

# Compute sha1sum of unsparsed recovery&recoveryfs images
# only after recoveryfs.ext4 image creation has already happened.
do_rootfs[depends] += "machine-recovery-image:do_makesystem"

IMAGE_INSTALL += "packagegroup-core-boot"

#IMAGE_INSTALL += "${ROOTFS_PKGMANAGE_BOOTSTRAP}"
IMAGE_INSTALL += "${CORE_IMAGE_EXTRA_INSTALL}"

IMAGE_INSTALL += "start-scripts-misc-daemon"

# The MSM Linux minimal boot image files.


IMAGE_INSTALL += "base-files"
IMAGE_INSTALL += "base-passwd"
#IMAGE_INSTALL += "bluez5"

IMAGE_INSTALL += "binder"

#Install coreutils for non-user builds.
IMAGE_INSTALL += "${@bb.utils.contains('VARIANT','user','','coreutils',d)}"

IMAGE_INSTALL += "glib-2.0"

IMAGE_INSTALL += "libnl"
IMAGE_INSTALL += "libcutils"
IMAGE_INSTALL += "liblog"

IMAGE_INSTALL += "libxml2"

#IMAGE_INSTALL += "start-scripts-find-partitions"
IMAGE_INSTALL += "start-scripts-firmware-links"
IMAGE_INSTALL += "system-core-adbd"
IMAGE_INSTALL += "system-core-logd"
IMAGE_INSTALL += "system-core-leprop"
IMAGE_INSTALL += "system-core-usb"
IMAGE_INSTALL += "system-core-post-boot"
IMAGE_INSTALL += "system-conf"
IMAGE_INSTALL += "system-prop"
IMAGE_INSTALL += "systemd-machine-units"
IMAGE_INSTALL += "wireless-tools"
IMAGE_INSTALL += "volatile-binds"

# Install recovery(fs)-updater only for non-A/B variants
IMAGE_INSTALL += "${@bb.utils.contains('DISTRO_FEATURES', 'ab-boot-support', '', 'start-scripts-recovery-updater', d)}"

# Install recovery and its libs in system image if A/B boot is supported
IMAGE_INSTALL += "${@bb.utils.contains('DISTRO_FEATURES', 'ab-boot-support', 'recovery-bin', '', d)}"
IMAGE_INSTALL += "${@bb.utils.contains('DISTRO_FEATURES', 'ab-boot-support', 'recovery', '', d)}"
#IMAGE_INSTALL += "${@bb.utils.contains('DISTRO_FEATURES', 'ab-boot-support', 'lib32-recovery', '', d)}"

# Don't install GDB and related pacakges in production builds.
DEPENDS += "${@base_conditional('USER_BUILD', '1', '', 'gdb-cross-arm', d)}"
IMAGE_INSTALL += "${@base_conditional('USER_BUILD', '1', '', 'gdb', d)}"
IMAGE_INSTALL += "${@base_conditional('USER_BUILD', '1', '', 'gdbserver', d)}"

# Location of unsparsed recoveryfs & recovery images
UNSPARSED_RECOVERYFS_DEST = "${TMPDIR}/work/x86_64-linux/releasetools-native/1.0-r0/releasetools/BOOTABLE_IMAGES"

# Compute sha1 of recovery(fs) images
# as an extension of the do_rootfs() task.
do_process_recovery_images() {
    # Create a "recoveryupgrade" directory which will house the
    # data needed by the recovery/recoveryfs upgrade mechanism.
    mkdir -p ${IMAGE_ROOTFS}/recoveryupgrade

    # first compute sha1 of boot.img, which is same as recovery partition's content
    /usr/bin/sha1sum ${DEPLOY_DIR_IMAGE}/${BOOTIMAGE_TARGET} | \
        awk '{ print $1 }' > ${IMAGE_ROOTFS}/recoveryupgrade/recoveryimg_sha1
    stat --printf="%s" ${DEPLOY_DIR_IMAGE}/${BOOTIMAGE_TARGET} > \
        ${IMAGE_ROOTFS}/recoveryupgrade/recoveryimg_length

    # now compute sha1 of the unsparsed recoveryfs image
    /usr/bin/sha1sum ${UNSPARSED_RECOVERYFS_DEST}/recovery-unsparsed.ext4 | \
        awk '{ print $1 }' > ${IMAGE_ROOTFS}/recoveryupgrade/recoveryfsimg_sha1
    stat --printf="%s" ${UNSPARSED_RECOVERYFS_DEST}/recovery-unsparsed.ext4 > \
        ${IMAGE_ROOTFS}/recoveryupgrade/recoveryfsimg_length
}

ROOTFS_POSTPROCESS_COMMAND += "do_process_recovery_images; "

# Below is to generate sparse ext4 system image (OE by default supports raw ext4 images)

do_makesystem() {
    cp ${THISDIR}/${BASEMACHINE}/msm8909-fsconfig.conf ${WORKDIR}/rootfs-fsconfig.conf
    make_ext4fs -C ${WORKDIR}/rootfs-fsconfig.conf -B ${DEPLOY_DIR_IMAGE}/system.map -s ${IMAGE_EXT4_SELINUX_OPTIONS} -b 4096 -l ${SYSTEM_SIZE_EXT4} ${DEPLOY_DIR_IMAGE}/${SYSTEMIMAGE_TARGET} ${IMAGE_ROOTFS}
    make_ext4fs  -b 4096 -a /systemrw ${IMAGE_EXT4_SELINUX_OPTIONS} -l ${SYSTEMRW_SIZE_EXT4} ${DEPLOY_DIR_IMAGE}/${SYSTEMRWIMAGE_TARGET}
}
