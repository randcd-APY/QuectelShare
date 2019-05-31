# BT proprietary Packages

IMAGE_INSTALL += "qc-bt-daemon"
IMAGE_INSTALL += "bt-services"

# Following packages support only 32-bit compilation.
# When 32-bit multilib is enabled, compile them with lib32- prefix.

IMAGE_INSTALL += "${@bb.utils.contains('MULTILIB_VARIANTS', 'lib32', 'lib32-hci-qcomm-init', 'hci-qcomm-init', d)}"
