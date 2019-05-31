# wlan proprietary Packages
IMAGE_INSTALL += "cnss-daemon"
IMAGE_INSTALL += "${@bb.utils.contains('TARGET_ARCH', 'aarch64','qcacld-utils','',d)}"
IMAGE_INSTALL += "ftm"
IMAGE_INSTALL += "ath6kl-utils"
