inherit autotools pkgconfig qlicense sdllvm qprebuilt

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/services/mm-camera-noship/"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/camera/services/mm-camera-noship/"

SRC_DIR = "${WORKSPACE}/camera/services/mm-camera-noship/"

def get_fastcv_depends(d):
    if d.getVar('BASEMACHINE', True) == 'apq8098':
        return ""
    else:
        return "fastcv-noship"

DEPENDS += "glib-2.0 libxml2 cameradbg"
DEPENDS += "${@get_fastcv_depends(d)}"
ENABLE_SDLLVM = "true"
TARGET_CFLAGS +="${THUMB_FLAGS} ${OPTIONAL_CFLAGS}"
TARGET_LDFLAGS +="${OPTIONAL_LDFLAGS} -avoid-version"

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
               --with-glib \
               --with-common-includes=${STAGING_INCDIR} \
               --with-camera-hal-path=${WORKSPACE}/camera/lib \
               --with-mm-camera-path=${WORKSPACE}/camera/services/mm-camera \
               --with-mm-camerasdk-path=${WORKSPACE}/camera/services/mm-camerasdk"

do_install_append() {
install -m 0644 ${S}/is/eis_dg/lib/libmesh_fusion.so -D ${D}${libdir}/libmesh_fusion.so
install -m 0644 ${S}/lcac/prebuilt/usr/lib/libcac_v4.so -D ${D}${libdir}/libcac_v4.so

if [ "${BASEMACHINE}" != "apq8096" ]; then
    install -m 0644 ${S}/is/eis_dg/lib/libmvDGTC.so -D ${D}${libdir}/libmvDGTC.so
fi
}
include ${BASEMACHINE}.inc
EXTRA_OECONF += "TARGET_NEON_ENABLED=false"

CPPFLAGS  = "-I ${STAGING_KERNEL_BUILDDIR}/usr/include/media"
FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/* \
                   /lib/firmware/* "
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
INSANE_SKIP_${PN} = "dev-so"
