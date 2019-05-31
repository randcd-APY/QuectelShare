# gfx proprietary source Packages

# Adreno can compile for both 32bit and 64bit.
# When multilib is enabled, compile with lib32- prefix.

IMAGE_INSTALL += "adreno"
IMAGE_INSTALL += "${@bb.utils.contains('MULTILIB_VARIANTS', 'lib32', 'lib32-adreno', 'adreno', d)}"
