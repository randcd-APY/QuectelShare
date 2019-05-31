inherit autotools pkgconfig qlicense sdllvm

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/lib"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/lib"

SRC_DIR = "${WORKSPACE}/camera/lib"

DEPENDS += "glib-2.0 "

def get_media_depends(d):
    if d.getVar('BASEMACHINE', True) == 'apq8098':
        return "system-media av-frameworks display-hal"
    else:
        return "media"
DEPENDS += "${@get_media_depends(d)}"

def config_camera_debug_data(d):
    if d.getVar('BASEMACHINE', True) == 'apq8096' and d.getVar('PRODUCT', True) == 'drone':
        return "CAMERA_DEBUG_DATA=true"
    else:
        return ""

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF += "--with-glib"
EXTRA_OECONF += "--with-common-includes=${STAGING_INCDIR}"
EXTRA_OECONF += "${@config_camera_debug_data(d)}"

ENABLE_SDLLVM = "true"
TARGET_CFLAGS +="${THUMB_FLAGS} ${OPTIONAL_CFLAGS}"
TARGET_LDFLAGS +="${OPTIONAL_LDFLAGS} -avoid-version"

include camera-${BASEMACHINE}.inc

FILES_${PN}-dbg  = "${libdir}/.debug/* /usr/bin/.debug/* /usr/lib/hw/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/* ${libdir}/hw/*.so"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
INSANE_SKIP_${PN} = "dev-so"
