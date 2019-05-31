DEPENDS += "virtual/kernel"

DEPENDS += "pkgconfig-native gtk-doc-native gettext-native mkbootimg-native"
DEPENDS += "bzip2 fsconfig-native applypatch-native bsdiff-native ext4-utils-native"

# The OE-Core minimal boot image files.
IMAGE_INSTALL += "packagegroup-core-boot"
IMAGE_INSTALL += "${CORE_IMAGE_EXTRA_INSTALL}"

# The MSM Linux minimal boot image files.
IMAGE_INSTALL += "base-files"
IMAGE_INSTALL += "base-passwd"
IMAGE_INSTALL += "system-core-adbd"
IMAGE_INSTALL += "system-core-logd"
IMAGE_INSTALL += "system-core-usb"
IMAGE_INSTALL += "recovery-bin"
IMAGE_INSTALL += "recovery"
IMAGE_INSTALL += "${@bb.utils.contains('MULTILIB_VARIANTS', 'lib32', 'lib32-recovery', '', d)}"
IMAGE_INSTALL += "powerapp"
IMAGE_INSTALL += "powerapp-powerconfig"
IMAGE_INSTALL += "powerapp-reboot"
IMAGE_INSTALL += "powerapp-shutdown"
IMAGE_INSTALL += "start-scripts-find-recovery-partitions"
IMAGE_INSTALL += "start-scripts-firmware-links"
IMAGE_INSTALL += "mtd-utils-ubifs"
IMAGE_INSTALL += "e2fsprogs-e2fsck"
IMAGE_INSTALL += "e2fsprogs-mke2fs"
IMAGE_INSTALL += "libcomerr"
IMAGE_INSTALL += "libss"
IMAGE_INSTALL += "libe2p"
IMAGE_INSTALL += "libext2fs"

# Needed for generating recovery.ubi ignored for ext4
MKUBIFS_ARGS = "${@bb.utils.contains('IMAGE_FSTYPES', 'ubi', '-m 4096 -e 253952 -c 83 -F', '', d)}"
UBI_VOLNAME  = "${@bb.utils.contains('IMAGE_FSTYPES', 'ubi', 'rootfs', '', d)}"
EXT4_IMAGET_TYPE  = "${@bb.utils.contains('IMAGE_FSTYPES', 'ext4', True, False, d)}"

do_fsconfig() {
 chmod go-r ${IMAGE_ROOTFS}/etc/passwd
 if ${@bb.utils.contains('DISTRO_FEATURES','systemd','false','true',d)}; then
  if [ "${DISTRO_NAME}" == "msm-user" ]; then
   if [ -e ${IMAGE_ROOTFS}/etc/init.d/sysfs.sh]; then
    sed -i '/mount -t debugfs/ d' ${IMAGE_ROOTFS}/etc/init.d/sysfs.sh
   fi
  fi
  chmod -R o-rwx ${IMAGE_ROOTFS}/etc/init.d/
 fi
}

UNSPARSED_RECOVERYFS_DEST = "${TMPDIR}/work/x86_64-linux/releasetools-native/1.0-r0/releasetools/BOOTABLE_IMAGES"

# Create a BOOTABLE_IMAGES dir in this location,
# so as to maintain directory structure for zipping
do_makesystem[cleandirs] += "${UNSPARSED_RECOVERYFS_DEST}"

# Below is to generate sparse ext4 recovery image (OE by default supports raw ext4 images)
do_makesystem() {
	if ${@bb.utils.contains('IMAGE_FSTYPES', 'ext4', 'true', 'false', d)}; then
        # SELinux feature is enabled
        if [[ "${DISTRO_FEATURES}" =~ "selinux" ]]; then
            make_ext4fs -s ${IMAGE_EXT4_SELINUX_OPTIONS} -l ${RECOVERYFS_SIZE_EXT4} ${DEPLOY_DIR_IMAGE}/${RECOVERYIMAGE_TARGET} ${IMAGE_ROOTFS}

            # Create an unsparse image as well to be included as part of ota target-files
            simg2img ${DEPLOY_DIR_IMAGE}/${RECOVERYIMAGE_TARGET} ${UNSPARSED_RECOVERYFS_DEST}/recovery-unsparsed.ext4
        else
            make_ext4fs -s -l ${RECOVERYFS_SIZE_EXT4} ${DEPLOY_DIR_IMAGE}/${RECOVERYIMAGE_TARGET} ${IMAGE_ROOTFS}

            # Create an unsparse image as well to be included as part of ota target-files
            simg2img ${DEPLOY_DIR_IMAGE}/${RECOVERYIMAGE_TARGET} ${UNSPARSED_RECOVERYFS_DEST}/recovery-unsparsed.ext4
        fi
	fi
}

do_rename_recovery_ubi_image() {
    #rename machine-recovery-image.ubi if it exists
    if [ -f "${DEPLOY_DIR_IMAGE}/machine-recovery-image-${MACHINE}.ubi" ]; then
        mv ${DEPLOY_DIR_IMAGE}/machine-recovery-image-${MACHINE}.ubi ${DEPLOY_DIR_IMAGE}/${MACHINE}-recoveryfs.ubi
    fi
}

ROOTFS_POSTPROCESS_COMMAND += "do_fsconfig; "

# Call function makesystem to generate sparse ext4 image
addtask makesystem after do_rootfs before do_build

# rename recovery images only after they are copied to DEPLOY_DIR
addtask rename_recovery_ubi_image after do_image_complete before do_build
