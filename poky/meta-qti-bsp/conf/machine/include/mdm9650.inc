BASEMACHINE ?= "mdm9650"
require conf/machine/include/basemachine.inc

SOC_FAMILY ?= "mdm9x50"
include conf/machine/include/soc-family.inc

# Pull in security flags
require ${COREBASE}/meta-qti-bsp/conf/distro/include/security_flags.inc

# Enable floating-point ABI to Hard-float
#DEFAULTTUNE = "cortexa8hf-neon"

include conf/machine/include/tune-cortexa8.inc

PREFERRED_PROVIDER_virtual/kernel = "linux-msm"
PREFERRED_VERSION_linux-msm = "4.9"

BBMASK = ".*/*/recipes-devtools/mtd/"

MACHINE_DISPLAY_WIDTH_PIXELS ?= "480"
MACHINE_DISPLAY_HEIGHT_PIXELS ?= "640"

SERIAL_CONSOLE ?= "${@['', '115200 ttyMSM0'][d.getVar('VARIANT', True) == ('' or 'debug')]}"

DISTRO ?= "mdm"

MACHINE_FEATURES += "kernel26 keyboard ipa-offload"

PAGE_SIZE ?= '4096'

# Max supported inodes
EXTRA_IMAGECMD ?= "-N 2048"

#UBINIZE Args
UBINIZE_ARGS ?= "-m 4096 -p 256KiB -s 4096"
#MKUBIFS Args
MKUBIFS_ARGS ?= "-m 4096 -e 253952 -c 2146 -F"
#Volume sizes
SYSTEM_VOLUME_SIZE ?= "90MiB"
CACHE_VOLUME_SIZE ?= "70MiB"
SYSTEMRW_VOLUME_SIZE ?= "6MiB"

# Sizes for EXT4 (in bytes)
SYSTEM_SIZE_EXT4 ?= "200000000"
SYSTEMRW_SIZE_EXT4 ?= "8388608"
USERDATA_SIZE_EXT4 ?= "2000000000"
CACHE_SIZE_EXT4 ?= "80000000"

#Enable SDK feature
#SDK_IMAGE ?= "1"

# Formats of root filesystem images. 
IMAGE_FSTYPES += "ext4 ubi"
