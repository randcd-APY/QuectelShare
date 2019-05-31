# This include forces the generation of persist image file for /persist partition

PERSIST_IMAGE_BASENAME    ?= "${MACHINE}-persist"
OUTPUT_PERSIST_FILE       ?= "${DEPLOY_DIR_IMAGE}/${PERSISTIMAGE_TARGET}"
PERSIST_IMAGE_ROOTFS_SIZE ?= "6536668"
PERSIST_IMAGE_ROOTFS       ?= "${TMPDIR}/rootfs/${PERSIST_IMAGE_BASENAME}"
OUTPUT_FILE_PERSIST_UBI   ?= "${DEPLOY_DIR_IMAGE}/${PERSIST_IMAGE_BASENAME}.ubi"
OUTPUT_FILE_PERSIST_UBIFS ?= "${DEPLOY_DIR_IMAGE}/${PERSIST_IMAGE_BASENAME}.ubifs"
PERSIST_UBINIZE_CFG       ?= "${DEPLOY_DIR_IMAGE}/ubinize_persist.cfg"

ROOTFS_POSTPROCESS_COMMAND_prepend = " create_persist_rootfs; sync_host_fs; "

# Generating persist image in EXT4 format
IMAGE_PREPROCESS_COMMAND += "${@bb.utils.contains('DISTRO_FEATURES', 'persist-image', 'create_persist_img;', '', d)}"

# Root image is created now
create_persist_rootfs() {
   PERSIST_DIR="${IMAGE_ROOTFS}/persist"
   rm -rf ${PERSIST_IMAGE_ROOTFS}
   mkdir -p ${PERSIST_IMAGE_ROOTFS}
   if [ "$(ls -A ${PERSIST_DIR})" ]; then
      mv ${PERSIST_DIR}/* ${PERSIST_IMAGE_ROOTFS}
   fi
}

sync_host_fs() {
   /bin/sync
}
create_persist_img () {
   if [[ "${IMAGE_FSTYPES}" =~ .*ext4* ]]; then
     create_persist_ext4_img
   fi
   if [[ "${DISTRO_FEATURES}" =~ .*nand-boot* ]]; then
    create_persist_ubi_img
   fi
}

create_persist_ext4_img() {
   make_ext4fs ${PERSISTFS_CONFIG} ${MAKEEXT4_MOUNT_OPT} -s -l ${PERSIST_IMAGE_ROOTFS_SIZE} ${OUTPUT_PERSIST_FILE} ${PERSIST_IMAGE_ROOTFS}
   chmod 644 ${OUTPUT_PERSIST_FILE}
}


create_persist_ubi_img() {
   mkfs.ubifs -r ${PERSIST_IMAGE_ROOTFS} -o ${OUTPUT_FILE_PERSIST_UBIFS} ${MKUBIFS_PERSIST_ARGS}
   if [ "${BASEMACHINE}" == "msm8909" ]; then
       if [ "${DISTRO}" == "quec-smart" ] || [ "${DISTRO}" == "robot-rome" ]; then
           create_ubinize_persist_config
           ubinize -o ${OUTPUT_FILE_PERSIST_UBI} ${UBINIZE_ARGS} ${PERSIST_UBINIZE_CFG}
           chmod 644 ${OUTPUT_FILE_PERSIST_UBIFS}
       fi
   fi
}

create_ubinize_persist_config() {
   echo \[persist_volume\] >> "${PERSIST_UBINIZE_CFG}"
   echo mode=ubi >> "${PERSIST_UBINIZE_CFG}"
   echo image="${OUTPUT_FILE_PERSIST_UBIFS}" >> "${PERSIST_UBINIZE_CFG}"
   echo vol_id=0 >> "${PERSIST_UBINIZE_CFG}"
   echo vol_type=dynamic >> "${PERSIST_UBINIZE_CFG}"
   echo vol_name=persist >> "${PERSIST_UBINIZE_CFG}"
   echo vol_size="${PERSIST_VOLUME_SIZE}" >> "${PERSIST_UBINIZE_CFG}"
}
