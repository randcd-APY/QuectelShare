inherit deploy qperf

DESCRIPTION = "Little Kernel bootloader"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/\
${LICENSE};md5=0835ade698e0bcf8506ecda2f7b4f302"
HOMEPAGE = "https://www.codeaurora.org/gitweb/quic/la?p=kernel/lk.git"

PROVIDES = "virtual/bootloader"

PR       = "r1"

PACKAGE_ARCH = "${MACHINE_ARCH}"
FILESPATH =+ "${WORKSPACE}:"

SRC_URI   =  "file://bootable/bootloader/lk"
S         =  "${WORKDIR}/bootable/bootloader/lk"

MY_TARGET_mdm9650 = "mdm9640"
MY_TARGET_sdx20 = "mdm9640"
MY_TARGET_msm8909  = "msm8909"
MY_TARGET_msm8909  = "msm8909"
MY_TARGET_msm8909w = "msm8909"
MY_TARGET_apq8096  = "msm8996"
MY_TARGET_mdm9607  = "mdm9607"
MY_TARGET_apq8053  = "msm8953"
MY_TARGET_apq8017  = "msm8952"
MY_TARGET         ?= "${BASEMACHINE}"

BOOTLOADER_NAME = "${@bb.utils.contains('DISTRO_FEATURES', 'emmc-boot', 'emmc_appsboot', 'appsboot', d)}"

emmc_bootloader = "${@bb.utils.contains('DISTRO_FEATURES', 'emmc-boot', '1', '0', d)}"

LIBGCC = "${STAGING_LIBDIR}/${TARGET_SYS}/6.4.0/libgcc.a"

# Disable display for nodisplay products
DISPLAY_SCREEN = "1"
DISPLAY_SCREEN_drone = "0"
DISPLAY_SCREEN_batcam = "0"
DISPLAY_SCREEN_quec-smart = "0"

ENABLE_DISPLAY = "${DISPLAY_SCREEN}"

EXTRA_OEMAKE = "${MY_TARGET} TOOLCHAIN_PREFIX='${TARGET_PREFIX}'  LIBGCC='${LIBGCC}' DISPLAY_SCREEN=${DISPLAY_SCREEN} ENABLE_DISPLAY=${ENABLE_DISPLAY}"

EXTRA_OEMAKE_append_mdm9650 = " ENABLE_EARLY_ETHERNET=1"

EXTRA_OEMAKE_append = " VERIFIED_BOOT=0 DEFAULT_UNLOCK=true EMMC_BOOT=${emmc_bootloader}"

EXTRA_OEMAKE_append = " ${@bb.utils.contains('DISTRO_FEATURES', 'ab-support', '', 'APPEND_CMDLINE=1', d)}"

EXTRA_OEMAKE_append = " ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'USE_LE_SYSTEMD=true', '', d)}"

EXTRA_OEMAKE_append = " ${@bb.utils.contains('DISTRO_FEATURES', 'vble', 'VERIFIED_BOOT_LE=1', '', d)}"

EXTRA_OEMAKE_append = " ${@bb.utils.contains('DISTRO_FEATURES', 'dm-verity', 'VERITY_LE=1', '', d)}"

EXTRA_OEMAKE_append_quec-smart = "TARGET_USE_QSEECOM_V4=1"

#enable hardfloat
EXTRA_OEMAKE_append = " ${@bb.utils.contains('TUNE_FEATURES', 'callconvention-hard', 'ENABLE_HARD_FPU=1', '', d)}"

#add more cflags to lk, if GCC6.3 version
EXTRA_OEMAKE_append = " 'LKLE_CFLAGS=-Wno-shift-negative-value -Wno-misleading-indentation -Wunused-const-variable=0 -DINIT_BIN_LE=\"/sbin/init\"' "

# Disable debug logs for non debug variant builds.
EXTRA_OEMAKE_append = " ${@bb.utils.contains('VARIANT', 'debug', '', 'DISABLE_LOGGING_BL=1', d)}"

EXTRA_OEMAKE_append = " ${@bb.utils.contains('VARIANT', 'user', 'TARGET_BUILD_VARIANT=user', '', d)}"

do_install() {
        install -d ${D}/boot
        install build-${MY_TARGET}/*.mbn ${D}/boot
}

FILES_${PN} = "/boot"
FILES_${PN}-dbg = "/boot/.debug"

do_deploy() {
        mkdir -p ${DEPLOY_DIR_IMAGE}
        install ${S}/build-${MY_TARGET}/*.mbn ${DEPLOY_DIR_IMAGE}
}

do_deploy[dirs] = "${S} ${DEPLOYDIR}"
addtask deploy before do_build after do_install

PACKAGE_STRIP = "no"
