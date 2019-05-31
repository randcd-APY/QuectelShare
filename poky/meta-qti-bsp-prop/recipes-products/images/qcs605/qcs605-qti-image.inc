# Packages installed to the root file system

IMAGE_INSTALL += "modem-shutdown"
#IMAGE_INSTALL += "msmirqbalance"
IMAGE_INSTALL += "start-scripts-misc-daemon"
IMAGE_INSTALL += "start-scripts-qseecomd-daemon"
IMAGE_INSTALL += "start-scripts-sfs-config"
IMAGE_INSTALL += "start-scripts-le-updater"

IMAGE_INSTALL += "adsprpc"
#IMAGE_INSTALL += "sensors"
IMAGE_INSTALL += "sensors-see-commonsys-intf"
IMAGE_INSTALL += "sensors-see"
IMAGE_INSTALL += "${@bb.utils.contains('MULTILIB_VARIANTS', 'lib32', 'lib32-sensors-see-commonsys-intf', '', d)}"
IMAGE_INSTALL += "${@bb.utils.contains('MULTILIB_VARIANTS', 'lib32', 'lib32-sensors-see', '', d)}"

IMAGE_INSTALL += "ss-services"
IMAGE_INSTALL_ATTEMPTONLY += "fastmmi"
