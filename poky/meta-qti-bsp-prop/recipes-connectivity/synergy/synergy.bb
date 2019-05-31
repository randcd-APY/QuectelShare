inherit qcommon qprebuilt qlicense
DESCRIPTION = "QTI Synergy BT Stack"

DEPENDS = "cmake-native mcm-core alsa-lib"

PACKAGE_ARCH = "${MACHINE_ARCH}"

INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

SRC_DIR = "${WORKSPACE}/synergy-bt-proprietary"
S = "${WORKDIR}/synergy-bt-proprietary"

ALLOW_EMPTY_${PN} = "1"

SRCREV = "${AUTOREV}"

LDFLAGS = "-Wl,-O1"

#inherit module update-rc.d
#INITSCRIPT_NAME = "synergy.sh"
#INITSCRIPT_PARAMS = "defaults 20"

#####################################################################################################

# Compile synergy-bt
do_compile_bt () {
    export CC="${CC}"
    export CROSS_COMPILE=${TARGET_PREFIX}

if [ -f "${B}/synergy-bt/platform/mdm/makefile" ]; then
    make -C ${B} -f ${B}/synergy-bt/platform/mdm/makefile all VARIANT="${VARIANT}" BASEMACHINE="${BASEMACHINE}" IMGRFS=${STAGING_DIR_HOST} CROSS_COMPILE=${TARGET_PREFIX} V=1 CC="${CC}"
fi
}

# Compile synergy-bt-sdk
do_compile_sdk () {
    export CC="${CC}"
    export CROSS_COMPILE=${TARGET_PREFIX}

    make -C ${B} -f ${B}/synergy-bt-sdk/platform/mdm/makefile all VARIANT="${VARIANT}" BASEMACHINE="${BASEMACHINE}" IMGRFS=${STAGING_DIR_HOST} CROSS_COMPILE=${TARGET_PREFIX} V=1 CC="${CC}"
}

# Compile synergy-middleware
do_compile_middleware () {
    export CC="${CC}"
    export CROSS_COMPILE=${TARGET_PREFIX}

    make -C ${B} -f ${B}/synergy-middleware/platform/mdm/makefile all IMGRFS=${STAGING_DIR_HOST} CROSS_COMPILE=${TARGET_PREFIX} V=1 CC="${CC}"
}

# Compile synergy-bootstrap
do_compile_bootstrap () {
    export CC="${CC}"
    export CROSS_COMPILE=${TARGET_PREFIX}

    make -C ${B} -f ${B}/synergy-bootstrap/platform/mdm/makefile all IMGRFS=${STAGING_DIR_HOST} CROSS_COMPILE=${TARGET_PREFIX} V=1 CC="${CC}"
}

#####################################################################################################

# Install synergy-bt-sdk
do_install_sdk () {
    install -d ${D}${bindir}

# Install synergy bt application.
    install ${B}/synergy-bt-sdk/common/synergy/output/host/bin/bt/app/csr_bt_app_default_hci ${D}${bindir}

# Install the script to start up synergy bt application.
    install ${B}/synergy-bt-sdk/platform/mdm/linux_oe_update/prebuilt/synergy.sh ${D}${bindir}

# Install the script to transfer hf audio. This is only valid for carkit.
    install ${B}/synergy-bt-sdk/platform/mdm/linux_oe_update/prebuilt/bt_hf_audio.sh ${D}${bindir}

# Install the script to transfer hfg audio. This is only valid for phone.
    install ${B}/synergy-bt-sdk/platform/mdm/linux_oe_update/prebuilt/bt_hfg_audio.sh ${D}${bindir}

# Install the script to start/stop mcm ril service. This is only valid for phone.
    install ${B}/synergy-bt-sdk/platform/mdm/linux_oe_update/prebuilt/mdm_ril_daemon.sh ${D}${bindir}

# Install the script to start/stop network configuration for pan. This is fitted with panu and nap.
if [ -f "${B}/synergy-bt-sdk/platform/mdm/linux_oe_update/prebuilt/bt_pan.sh" ]; then
    install ${B}/synergy-bt-sdk/platform/mdm/linux_oe_update/prebuilt/bt_pan.sh ${D}${bindir}
fi
}

# Install synergy-bootstrap
do_install_bootstrap () {
    install -d ${D}${bindir}
    install -d ${D}/etc/bluetooth

# Install bt_bootstrap for QCA chip.
    install ${B}/synergy-bootstrap/output/host/bin/bt_bootstrap ${D}${bindir}

# Install bt firmware patch for QCA chip.
if [ -d "${B}/synergy-bootstrap/platform/mdm/bt_firmware_patch/qca" ]; then
    install ${B}/synergy-bootstrap/platform/mdm/bt_firmware_patch/qca/* ${D}/etc/bluetooth
fi

# Install bt_test application for QCA chip.
if [ -f "${B}/synergy-bootstrap/output/host/bin/bt_test" ]; then
    install ${B}/synergy-bootstrap/output/host/bin/bt_test ${D}${bindir}
fi

# Install btdiag application for QCA chip.
if [ -f "${B}/synergy-bootstrap/output/host/bin/btdiag" ]; then
    install ${B}/synergy-bootstrap/output/host/bin/btdiag ${D}${bindir}
fi
}

# Install synergy-middleware
do_install_middleware() {
    install -d ${D}${bindir}

# Install synergy bt audio service
    install ${B}/synergy-middleware/output/host/bin/bt_audio_service ${D}${bindir}
}
#####################################################################################################

do_compile () {
    do_compile_bootstrap

    do_compile_bt
    do_compile_middleware
    do_compile_sdk
}

do_install () {
    do_install_bootstrap

    # nothing to do_install_bt
    do_install_middleware
    do_install_sdk
}

FILES_${PN} += "usr/bin/"
FILES_${PN} += "usr/lib/"
FILES_${PN} += "etc/bluetooth/"
