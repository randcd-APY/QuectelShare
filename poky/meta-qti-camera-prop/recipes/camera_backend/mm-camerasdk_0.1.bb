inherit autotools pkgconfig qlicense qprebuilt sdllvm

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/services/mm-camerasdk/"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/camera/services/mm-camerasdk/"

SRC_DIR = "${WORKSPACE}/camera/services/mm-camerasdk/"

DEPENDS += "glib-2.0 libxml2 system-core"

ENABLE_SDLLVM = "true"
TARGET_CFLAGS +="${THUMB_FLAGS} ${OPTIONAL_CFLAGS}"
TARGET_LDFLAGS +="${OPTIONAL_LDFLAGS} -avoid-version"

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
               --with-glib \
               --with-common-includes=${STAGING_INCDIR}"

def get_platform(d):
    if d.getVar('BASEMACHINE', True) == 'apq8053':
        return "TARGET_BOARD_PLATFORM=8953"
    elif d.getVar('BASEMACHINE', True) == 'apq8096':
        return "TARGET_BOARD_PLATFORM=8996"
    elif d.getVar('BASEMACHINE', True) == 'apq8098':
        return "TARGET_BOARD_PLATFORM=8998"
    else:
        return ""
include ${BASEMACHINE}.inc
EXTRA_OECONF += "${@get_platform(d)}"
do_install_append(){
    install -d ${D}/usr/bin
}

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/* \
                   /lib/firmware/* "
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
INSANE_SKIP_${PN} = "dev-so"
