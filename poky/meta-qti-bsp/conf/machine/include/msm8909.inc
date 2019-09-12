BASEMACHINE ?= "msm8909"
require conf/machine/include/basemachine.inc

SOC_FAMILY ?= "msm8909"
include conf/machine/include/soc-family.inc

# Pull in security flags
require ${COREBASE}/meta-qti-bsp/conf/distro/include/security_flags.inc

DEFAULTTUNE ?= "armv7ahf-neon"

include conf/machine/include/tune-cortexa7.inc

PREFERRED_PROVIDER_virtual/kernel = "linux-msm"
PREFERRED_VERSION_linux-msm = "3.18"

MACHINE_DISPLAY_WIDTH_PIXELS ?= "480"
MACHINE_DISPLAY_HEIGHT_PIXELS ?= "640"

SERIAL_CONSOLE ?= "${@['', '115200 ttyHSL0'][d.getVar('VARIANT', True) == ('' or 'debug')]}"

DISTRO ?= "msm"

DISTRO_FEATURES += "persist-image"

MACHINE_FEATURES += "kernel26 keyboard"

PAGE_SIZE ?= '4096'

# Max supported inodes
EXTRA_IMAGECMD ?= "-N 2048"

# Sizes for EXT4 (in bytes)
SYSTEM_SIZE_EXT4 ?= "1610613760"
SYSTEMRW_SIZE_EXT4 ?= "8388608"
USERDATA_SIZE_EXT4 ?= "4755012608"
CACHE_SIZE_EXT4 ?= "536870912"
RECOVERYFS_SIZE_EXT4 ?= "268435456"

#Enable Recovery image generation
RECOVERY_IMAGE ?= "1"

# Formats of root filesystem images.
IMAGE_FSTYPES += "ext4"

#Enable SDK feature
# SDK_IMAGE ?= "1"
