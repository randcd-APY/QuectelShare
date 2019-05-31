inherit autotools qlicense

DESCRIPTION = "mm-camera-interface test app"

PR = "r1"

SRC_URI = "file://${WORKSPACE}/mm-camera/apps/mm-qcamera-app"

S = "${WORKDIR}/mm-qcamera-app"

# Need the kernel headers
DEPENDS += "virtual/kernel"
DEPENDS += "glib-2.0"
DEPENDS += "mm-camera"
DEPENDS += "camera-hal"

PACKAGE_ARCH = "${MACHINE_ARCH}"

ARM_INSTRUCTION_SET = "arm"

EXTRA_OECONF_append = " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF_append = " --with-mm-still=${WORKSPACE}/mm-still"
EXTRA_OECONF_append = " --with-common-includes=${STAGING_INCDIR}"
EXTRA_OECONF_append = " --enable-target=${BASEMACHINE} "

FILES_${PN} += "\
    /usr/bin/*"
