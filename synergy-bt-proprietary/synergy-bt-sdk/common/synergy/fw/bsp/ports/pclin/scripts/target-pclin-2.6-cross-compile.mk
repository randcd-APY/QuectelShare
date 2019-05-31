include $(TARGET_SCRIPT_ROOT)/target-default.mk

CC :=/home/chaoli/work/new_tooltrain_kernel_3.9/prebuilt/linux-x86/toolchain/gcc-linaro-arm-linux-gnueabihf-4.8-2013.05_linux/bin/arm-linux-gnueabihf-gcc
LD := /home/chaoli/work/new_tooltrain_kernel_3.9/prebuilt/linux-x86/toolchain/gcc-linaro-arm-linux-gnueabihf-4.8-2013.05_linux/bin/arm-linux-gnueabihf-ld
AR := /home/chaoli/work/new_tooltrain_kernel_3.9/prebuilt/linux-x86/toolchain/gcc-linaro-arm-linux-gnueabihf-4.8-2013.05_linux/bin/arm-linux-gnueabihf-ar
CXX := /home/chaoli/work/new_tooltrain_kernel_3.9/prebuilt/linux-x86/toolchain/gcc-linaro-arm-linux-gnueabihf-4.8-2013.05_linux/bin/arm-linux-gnueabihf-cpp
#CC := /home/chaoli/work/repo_a6_release/prebuilt/linux-x86/toolchain/linaro-201105-csr-build-armv7-vfpv3/bin/arm-none-linux-gnueabi-gcc
#LD := /home/chaoli/work/repo_a6_release/prebuilt/linux-x86/toolchain/linaro-201105-csr-build-armv7-vfpv3/bin/arm-none-linux-gnueabi-ld
#AR := /home/chaoli/work/repo_a6_release/prebuilt/linux-x86/toolchain/linaro-201105-csr-build-armv7-vfpv3/bin/arm-none-linux-gnueabi-ar
#CXX := /home/chaoli/work/repo_a6_release/prebuilt/linux-x86/toolchain/linaro-201105-csr-build-armv7-vfpv3/bin/arm-none-linux-gnueabi-cpp

MODE=user

SUB_TARGET_ARCH := Linux
SUB_TARGET_VERSION := 2.6

TARGET_LIBS := pthread rt


