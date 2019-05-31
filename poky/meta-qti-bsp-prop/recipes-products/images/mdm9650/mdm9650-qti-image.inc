# Packages installed to the root file system

IMAGE_INSTALL += "mcm-core"
IMAGE_INSTALL += "modem-shutdown"
IMAGE_INSTALL += "start-scripts-misc-daemon"
#IMAGE_INSTALL += "subsystem-ramdump"
#IMAGE_INSTALL += "qti-logkit"

SYNERGY_DIR_PRESENT = "${@os.path.exists('${WORKSPACE}/prebuilt_HY11/mdm9650/synergy') or os.path.exists('${WORKSPACE}/prebuilt_HY22/mdm9650/synergy') or os.path.exists('${WORKSPACE}/synergy-bt-proprietary')}"
IMAGE_INSTALL += "${@base_conditional('SYNERGY_DIR_PRESENT', 'True', 'synergy', '', d)}"
