inherit autotools qcommon qlicense qprebuilt

DESCRIPTION = "Adreno Graphics"
PR = "r0"
PROVIDES = "adreno200"

##### Display Frameworks #####
# Set this to 1, if Android display framework is supported
SUPPORTS_ANDROID_FRAMEWORK = "1"

# Set this to 1, if Wayland display framework is supported
SUPPORTS_WAYLAND_FRAMEWORK = "0"

# Set this to 1, if FBDEV is supported
SUPPORTS_FBDEV = "0"

##### Kernel Mode Drivers #####
#Set this to 1, if KGSL is supported
SUPPORTS_KGSL = "1"

#Set this to 1, if DRM is supported
SUPPORTS_DRM = "0"

#Set this to 1, if Dynamic Loading is supported
SUPPORTS_DYNAMIC_SYMBOL_LOADING = "0"

#Set this to A3X/A5X/A6X, based on which GPU family is supported
SUPPORTS_GPU = "A5X"

#Set this to 1, if C2D is supported
SUPPORTS_C2D = "0"

#Set this to 1, if build is for Vipertooth.
VIPERTOOTH_BUILD = "0"

##### Target specific setting #####
# APQ8098 uses DRM kernel with Wayland display framework
SUPPORTS_ANDROID_FRAMEWORK_apq8098 = "0"
SUPPORTS_WAYLAND_FRAMEWORK_apq8098 = "1"
SUPPORTS_FBDEV_apq8098 = "0"
SUPPORTS_KGSL_apq8098  = "0"
SUPPORTS_DRM_apq8098   = "1"
SUPPORTS_GPU_apq8098   = "A5X"
SUPPORTS_DYNAMIC_SYMBOL_LOADING_apq8098  = "1"

# QCS605 uses DRM kernel with Wayland display framework
SUPPORTS_ANDROID_FRAMEWORK_qcs605 = "0"
SUPPORTS_WAYLAND_FRAMEWORK_qcs605 = "1"
SUPPORTS_FBDEV_qcs605 = "0"
SUPPORTS_KGSL_qcs605  = "1"
SUPPORTS_DRM_qcs605   = "0"
SUPPORTS_GPU_qcs605   = "A6X"
SUPPORTS_C2D_qcs605   = "1"

# APQ8017 uses C2D APIs
SUPPORTS_C2D_apq8017   = "1"

# QCS40x uses kgsl kernel with wayland display framework
VIPERTOOTH_BUILD_qcs40x            = "1"
SUPPORTS_ANDROID_FRAMEWORK_qcs40x  = "0"
SUPPORTS_WAYLAND_FRAMEWORK_qcs40x  = "1"
SUPPORTS_DRM_qcs40x                = "0"
SUPPORTS_FBDEV_qcs40x              = "0"
SUPPORTS_C2D_qcs40x                = "1"

INCSUFFIX             = "adreno-cmake"
INCSUFFIX_msm8909     = "adreno-makefile-linux"
INCSUFFIX_apq8017     = "adreno-makefile-linux"
INCSUFFIX_qcs40x      = "adreno-cmake"

#Set this for Sstate feature
do_configure[depends] += "virtual/kernel:do_shared_workdir"

include ${INCSUFFIX}.inc
