# Proprietary Packages installed to the root file system
IMAGE_INSTALL += "ss-services"

IMAGE_INSTALL += "adsprpc"
IMAGE_INSTALL += "lib32-adsprpc"
IMAGE_INSTALL += "${@bb.utils.contains('DISTRO_FEATURES', 'ab-boot-support', 'start-scripts-le-updater', '', d)}"
IMAGE_INSTALL += "sensors-see-commonsys-intf"
IMAGE_INSTALL += "sensors-see"
IMAGE_INSTALL_remove_qcs403-som2 += "lib32-adsprpc"
IMAGE_INSTALL_remove_qcs403-som2 += "sensors-see-commonsys-intf"
IMAGE_INSTALL_remove_qcs403-som2 += "sensors-see"
IMAGE_INSTALL_ATTEMPTONLY += "fastmmi"
