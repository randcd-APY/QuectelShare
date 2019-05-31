# Packages installed to the root file system

IMAGE_INSTALL += "mcm-core"
IMAGE_INSTALL += "modem-shutdown"
IMAGE_INSTALL += "qmuxd"
IMAGE_INSTALL += "subsystem-ramdump"
IMAGE_INSTALL += "start-scripts-misc-daemon"
#IMAGE_INSTALL += "start-scripts-mssboot"
IMAGE_INSTALL += "qti-logkit"
IMAGE_INSTALL += "synergy"

include ${BASEMACHINE}-${VARIANT}-only-packages.inc
