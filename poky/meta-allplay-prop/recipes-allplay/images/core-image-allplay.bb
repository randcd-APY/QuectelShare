SUMMARY = "Image for AllPlay"

LICENSE = "CLOSED"

IMAGE_INSTALL = "\
    packagegroup-core-boot \
    ${ROOTFS_PKGMANAGE_BOOTSTRAP} \
    \
    orbplayer \
    sam-system \
    \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    "

# If this doesn't work, you can add the follwing to your local.conf file to
# try the Intel card instead
#    ALLPLAY_SOUNDCARD = "snd-intel8x0"
ALLPLAY_SOUNDCARD ?= "snd-ens1370"

IMAGE_INSTALL_append_qemux86 = " kernel-module-${ALLPLAY_SOUNDCARD}"
IMAGE_INSTALL_append_qemux86-64 = " kernel-module-${ALLPLAY_SOUNDCARD}"

inherit core-image
