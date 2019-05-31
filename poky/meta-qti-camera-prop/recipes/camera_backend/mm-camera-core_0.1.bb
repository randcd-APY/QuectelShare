inherit autotools pkgconfig qlicense qprebuilt sdllvm

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/services/mm-camera-core"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/camera/mm-camera-core"

SRC_DIR = "${WORKSPACE}/camera/services/mm-camera-core"

DEPENDS += "glib-2.0 libxml2 cameradbg camera mm-camerasdk"

ENABLE_SDLLVM = "true"
TARGET_CFLAGS +="${THUMB_FLAGS} ${OPTIONAL_CFLAGS}"
TARGET_LDFLAGS +="${OPTIONAL_LDFLAGS} -avoid-version"

EXTRA_OEMAKE = "DEFAULT_INCLUDES= CPPFLAGS="-I. -I${STAGING_KERNEL_BUILDDIR}/usr/include""
EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
            --with-glib \
                --with-camera-hal-path=${WORKSPACE}/camera/lib \
                --with-mm-camera-path=${WORKSPACE}/camera/services/mm-camera \
                --with-mm-3a-core-path=${WORKSPACE}/camera/services/mm-3a-core \
                --with-mm-camerasdk-path=${WORKSPACE}/camera/services/mm-camerasdk"

include ${BASEMACHINE}.inc
include Boardconfig-${MACHINE}.inc

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
INSANE_SKIP_${PN} = "dev-so"
