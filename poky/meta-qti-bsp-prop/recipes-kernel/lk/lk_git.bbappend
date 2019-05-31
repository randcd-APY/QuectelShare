inherit qlicense qsigning

DESCRIPTION = "Sign the Little Kernel bootloader"
PR = "r1signed"

FILES_${PN} = "/boot"

EXTRA_OEMAKE_append = " SIGNED_KERNEL=1"

# This install overrides the one in base recipe. In this we perform image signing
# using the tools located in signing_tools_dir.
#
do_install() {
    install -d ${D}/boot

    # This performs signing the image at lk/build folder. The final image
    # gets written in the folder image/boot/. Subsequently deploy stage in the
    # main recipe installs to the DEPLOYDIR where rest of the system images are located.
    #
    SECIMAGE_LOCAL_DIR=${SIGNING_TOOLS_DIR}/SecImage \
    USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN=1 \
    USES_SEC_POLICY_INTEGRITY_CHECK=1 \
    python ${SIGNING_TOOLS_DIR}/SecImage/sectools_builder.py \
        -i build-${MY_TARGET}/${BOOTLOADER_NAME}.mbn \
        -t ${D}/boot \
        -g appsbl \
        --install_base_dir=${D}/boot \
            > ${S}/secimage.log 2>&1

    install ${D}/boot/${BOOTLOADER_NAME}.mbn ${S}/build-${MY_TARGET}

}

