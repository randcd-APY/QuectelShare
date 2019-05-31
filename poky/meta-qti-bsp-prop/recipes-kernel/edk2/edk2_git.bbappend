DESCRIPTION = "Sign the edk2 bootloader"
LICENSE           = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM  = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/${LICENSE};md5=92b1d0ceea78229551577d4284669bb8"
PR = "r1signed"

FILES_${PN} = "/boot"

SIGNING_TOOLS_DIR = "${TMPDIR}/work-shared/signing_tools"

DEPENDS += "sectool5-native openssl-native llvm-arm-toolchain-native"

BL_PLATFORM = "msmcobalt"
BL_PLATFORM_qcs605 = "qcs605"
BL_PLATFORM_sdxpoorwills = "sdxpoorwills"

T_ARCH = "arm"
T_ARCH_aarch64 = "aarch64"

EXTRA_OEMAKE_append += "BOOTLOADER_PLATFORM=${BL_PLATFORM}"
EXTRA_OEMAKE_append += "SIGNED_KERNEL=1"
EXTRA_OEMAKE_append += "USER_BUILD_VARIANT=0"
EXTRA_OEMAKE_append += "DEVICE_STATUS=DEFAULT_UNLOCK=true"
EXTRA_OEMAKE_append += "TARGET_ARCHITECTURE=${T_ARCH}"

SOC_HW_VERSION_qcs605 = "0x60040000"
SOC_HW_VERSION_sdxpoorwills = "0x60020000"
SOC_HW_VERSION_apq8098 = "0x30020000"
SOC_HW_VERSION_qcs40x = "0x20140100"
SOC_HW_VERSION_sdmsteppe = "0x60070000"

SOC_VERS_qcs605 = "0x6005 0x6009 0x600A"
SOC_VERS_sdxpoorwills = "0x6002"
SOC_VERS_apq8098 = "0x3005"
SOC_VERS_qcs40x ="0x2014"
SOC_VERS_sdmsteppe = "0x6007"

SIGNING_VERSION = "_eccv3"
SIGNING_VERSION_qcs40x = "v3"
SIGNING_VERSION_qcs605 = "v2"
SIGNING_VERSION_apq8098 = "v2"

do_install[depends] += "sectool5-native:do_unpack"

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
        -i ${WORKDIR}/abl.elf \
        -t ${D}/boot \
        -g abl \
        --soc_hw_version ${SOC_HW_VERSION} \
        --soc_vers ${SOC_VERS} \
        --config=${SIGNING_TOOLS_DIR}/SecImage/config/integration/secimage${SIGNING_VERSION}.xml \
        --install_base_dir=${D}/boot \
            > ${S}/secimage.log 2>&1

    install ${D}/boot/abl.elf ${S}

}

