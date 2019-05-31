inherit autotools

DESCRIPTION = "Adreno200 X11 Subdriver"
HOMEPAGE = "http://support.cdmatech.com"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

PR = "r10"

SRC_URI = "file://${WORKSPACE}/adreno-subdriver-x11"
S = "${WORKDIR}/adreno-subdriver-x11"

DEPENDS += "adreno"
DEPENDS += "libdrm libtbm libgenlock"

EXTRA_OECONF_append += " --with-adreno=${STAGING_DIR_TARGET}/usr"
EXTRA_OECONF_append += " --with-kernel-headers=${STAGING_KERNEL_DIR}/usr/include"
EXTRA_OECONF_append += " --with-genlock=${STAGING_DIR_TARGET}/usr"

PACKAGE_ARCH = "${MACHINE_ARCH}"

LEAD_SONAME="eglsubX11.so"
FILES_${PN} += "/usr/lib/egl/eglsubX11.so"

CFLAGS += " -Wno-error "
