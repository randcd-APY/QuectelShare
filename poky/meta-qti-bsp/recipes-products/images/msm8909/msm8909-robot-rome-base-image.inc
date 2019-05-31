#Include the robot settings

# Get base product settings
require msm8909-base-image.inc
# These two lines below is a workaround to remove init-mss,
# since packagegroup-ss-mgr just include reboot-daemon and init-mss
IMAGE_INSTALL_remove = "packagegroup-ss-mgr"
IMAGE_INSTALL += "reboot-daemon"
IMAGE_INSTALL += "e2fsprogs"
IMAGE_INSTALL += "start-scripts-userfs-reset"
IMAGE_INSTALL += "start-script-ab-slot-util"
IMAGE_INSTALL += "chrony"
IMAGE_INSTALL += "chronyc"

IMAGE_INSTALL += "openssh"
IMAGE_INSTALL += "iw"
IMAGE_INSTALL += "sigma-dut"
IMAGE_INSTALL += "libcap"
IMAGE_INSTALL += "attr"
IMAGE_INSTALL += "mtd-utils-ubifs"
IMAGE_INSTALL += "kernel-modules"
# Following packages support audio-dlkm enabled

AUDIOMODULE_FLAG = "${@bb.utils.contains('DISTRO_FEATURES', 'audio-dlkm', 'True', 'False', d)}"
IMAGE_INSTALL += "${@base_conditional('AUDIOMODULE_FLAG', 'True', bb.utils.contains('MULTILIB_VARIANTS', 'lib32', 'lib32-audiodlkm', '', d), '', d)}"
