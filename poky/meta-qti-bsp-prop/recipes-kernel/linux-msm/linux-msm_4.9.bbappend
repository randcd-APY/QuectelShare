FILESEXTRAPATHS_prepend := "${THISDIR}/${PV}:"

SRC_URI += "file://qti.key"
SRC_URI += "file://test_cvm_private_key_4096.pem"
SRC_URI += "file://vble-qti.key"
TARGET_SHA_TYPE = "sha256"

DEPENDS += "openssl-native avbtool"

do_deploy_append() {
    #Generating signed boot.img
    cd ${DEPLOY_DIR_IMAGE}
    if ${@bb.utils.contains('DISTRO_FEATURES', 'avble', 'true', 'false', d)}; then
       # Skip zero padding if avbtool is used to sign the image
       ${TMPDIR}/work-shared/avbtool/avbtool add_hash_footer --image ${BOOTIMAGE_TARGET} --partition_size 0x02000000 --partition_name vm-linux --algorithm SHA256_RSA4096 --key ${WORKDIR}/test_cvm_private_key_4096.pem --rollback_index 0
    else
       cp ${BOOTIMAGE_TARGET} ${BOOTIMAGE_TARGET}.nonsecure
       openssl dgst -${TARGET_SHA_TYPE} -binary ${BOOTIMAGE_TARGET}.nonsecure > ${BOOTIMAGE_TARGET}.${TARGET_SHA_TYPE}
       if ${@bb.utils.contains('DISTRO_FEATURES', 'vble', 'true', 'false', d)}; then
           openssl pkeyutl -sign -in ${BOOTIMAGE_TARGET}.${TARGET_SHA_TYPE} -inkey ${WORKDIR}/vble-qti.key -out ${BOOTIMAGE_TARGET}.sig -pkeyopt digest:${TARGET_SHA_TYPE} -pkeyopt rsa_padding_mode:pkcs1
       else
           openssl rsautl -sign -in ${BOOTIMAGE_TARGET}.${TARGET_SHA_TYPE} -inkey ${WORKDIR}/qti.key -out ${BOOTIMAGE_TARGET}.sig
       fi
       dd if=/dev/zero of=${BOOTIMAGE_TARGET}.sig.padded bs=2048 count=1
       dd if=${BOOTIMAGE_TARGET}.sig of=${BOOTIMAGE_TARGET}.sig.padded conv=notrunc
       cat ${BOOTIMAGE_TARGET}.nonsecure ${BOOTIMAGE_TARGET}.sig.padded > ${BOOTIMAGE_TARGET}.secure
       rm -rf ${BOOTIMAGE_TARGET}.${TARGET_SHA_TYPE} ${BOOTIMAGE_TARGET}.sig ${BOOTIMAGE_TARGET}.sig.padded
       mv -f ${BOOTIMAGE_TARGET}.secure ${BOOTIMAGE_TARGET}
    fi
}
