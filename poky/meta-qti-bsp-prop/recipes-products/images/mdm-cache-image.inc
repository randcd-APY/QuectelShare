# This include forces the generation of a /cache cache partition image file

OUTPUT_CACHE_FILE_EXT4 = "${DEPLOY_DIR_IMAGE}/${CACHEIMAGE_TARGET}"

# Generating cache image in EXT4 format

IMAGE_PREPROCESS_COMMAND_append = "${@bb.utils.contains('IMAGE_FSTYPES','ext4',' create_cache_ext4_img; ','', d)}"

create_cache_ext4_img() {
        make_ext4fs -s -l ${CACHE_SIZE_EXT4} ${OUTPUT_CACHE_FILE_EXT4}
        chmod 644 ${OUTPUT_CACHE_FILE_EXT4}
}
