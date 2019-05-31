inherit autotools pkgconfig qlicense qprebuilt

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/services/mm-camera-core-legacy/"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/camera/services/mm-camera-core-legacy/"

SRC_DIR = "${WORKSPACE}/camera/services/mm-camera-core-legacy/"

DEPENDS += "glib-2.0 libxml2 rb-camera"

EXTRA_OEMAKE = "DEFAULT_INCLUDES= CPPFLAGS="-I. -I${STAGING_KERNEL_BUILDDIR}/usr/include""

EXTRA_OECONF = "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                --with-glib \
                --with-camera-hal-path=${WORKSPACE}/camera/lib-legacy \
                --with-mm-camera-path=${WORKSPACE}/camera/services/mm-camera-legacy \
                --with-mm-camerasdk-path=${WORKSPACE}/camera/services/mm-camerasdk"

#include Boardconfig-${MACHINE}.inc

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
INSANE_SKIP_${PN} = "dev-so"
