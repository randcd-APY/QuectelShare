require ${COREBASE}/meta-qti-bsp/recipes-products/images/common/common-base-image.inc

#compile abl
RDEPENDS  += "${@bb.utils.contains('MULTILIB_VARIANTS', 'lib64','lib64-edk2','edk2',d)}"

#install userspace packages
IMAGE_INSTALL += "abctl"
IMAGE_INSTALL += "av-frameworks"
IMAGE_INSTALL += "binder"
IMAGE_INSTALL += "kptr-restrict"
IMAGE_INSTALL += "libion"
IMAGE_INSTALL += "libhardware"
IMAGE_INSTALL += "media-headers"
IMAGE_INSTALL += "start-script-ab-slot-util"
IMAGE_INSTALL += "start-scripts-firmware-links"
IMAGE_INSTALL += "system-conf"
IMAGE_INSTALL += "system-media"
IMAGE_INSTALL += "system-prop"
IMAGE_INSTALL += "wireless-tools"
IMAGE_INSTALL += "iw"
