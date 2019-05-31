inherit autotools pkgconfig qlicense sdllvm qprebuilt

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/services/mm-3a-core/"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/camera/services/mm-3a-core/"

SRC_DIR = "${WORKSPACE}/camera/services/mm-3a-core/"

DEPENDS += "glib-2.0 libxml2 mm-camera-core mm-camera-lib"

ENABLE_SDLLVM = "true"
TARGET_CFLAGS +="${THUMB_FLAGS} ${OPTIONAL_CFLAGS}"
TARGET_LDFLAGS +="${OPTIONAL_LDFLAGS} -avoid-version"

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
               --with-glib \
               --with-common-includes=${STAGING_INCDIR} \
               --with-camera-hal-path=${WORKSPACE}/camera/lib \
               --with-mm-camera-path=${WORKSPACE}/camera/services/mm-camera \
               --with-mm-camerasdk-path=${WORKSPACE}/camera/services/mm-camerasdk"

def get_platform(d):
    if d.getVar('BASEMACHINE', True) == 'apq8053':
        return "TARGET_BOARD_PLATFORM=8953"
    elif d.getVar('BASEMACHINE', True) == 'apq8096':
        return "TARGET_BOARD_PLATFORM=8996"
    elif d.getVar('BASEMACHINE', True) == 'apq8098':
        return "TARGET_BOARD_PLATFORM=8998"
    else:
        return ""

EXTRA_OECONF += "${@get_platform(d)}"
include ${BASEMACHINE}.inc

do_compile_prepend() {

if [ "${BASEMACHINE}" == "apq8053" ]; then
TARGET_PRODUCT=msm8953
fi

if [ "${BASEMACHINE}" == "apq8096" ]; then
TARGET_PRODUCT=msm8996
fi

if [ "${BASEMACHINE}" == "apq8098" ]; then
TARGET_PRODUCT=msm8998
fi

LOCAL_CORE_PATH=${S}
TARGET_BUILD_TYPE="release"
TARGET_OUT_HEADERS=${S}
MM_3A_CORE="mm-3a-core"
AU_SEARCH_ON=1

${S}/version.sh ${LOCAL_CORE_PATH} ${TARGET_PRODUCT} ${TARGET_BUILD_TYPE} ${TARGET_OUT_HEADERS} ${MM_3A_CORE} ${AU_SEARCH_ON}

}

do_install_append() {
install -d ${D}${includedir}/mm-3a-core
install -m 0644 ${S}/mm-3a-core/version.h -D ${D}/usr/include/mm-3a-core/
}

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/* \
                   /lib/firmware/* "
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
INSANE_SKIP_${PN} = "dev-so"
