# gfx proprietary source Packages

# Adreno can compile for both 32bit and 64bit.
# When multilib is enabled, compile both variants.
IMAGE_INSTALL += "adreno"
IMAGE_INSTALL += "lib32-adreno"
IMAGE_INSTALL_remove_qcs403-som2 += "adreno"
IMAGE_INSTALL_remove_qcs403-som2 += "lib32-adreno"
