inherit autotools pkgconfig qlicense sdllvm qprebuilt

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/services/mm-camera/mm-camera2/log_debug/"
SRC_URI   += "file://camera/services/mm-camera/mm-camera2/media-controller/mct/debug/"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/camera/services/mm-camera/mm-camera2/log_debug/"

SRC_DIR = "${WORKSPACE}/camera/services/mm-camera/mm-camera2/log_debug/"

DEPENDS += "glib-2.0 libcutils libxml2 camera"

ENABLE_SDLLVM = "true"
TARGET_LDFLAGS +="-avoid-version"

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
               --with-glib \
               --with-common-includes=${STAGING_INCDIR} \
               --with-core-includes=${WORKSPACE}/system/core/include \
               --with-camera-hal-path=${WORKSPACE}/camera/lib \
               --with-mm-camera-path=${WORKSPACE}/camera/services/mm-camera"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
INSANE_SKIP_${PN} = "dev-so"
