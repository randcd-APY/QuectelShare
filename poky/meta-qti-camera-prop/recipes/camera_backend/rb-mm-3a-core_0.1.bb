inherit autotools pkgconfig qlicense qprebuilt

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"
FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/services/mm-3a-core-legacy/"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/camera/services/mm-3a-core-legacy/"

SRC_DIR = "${WORKSPACE}/camera/services/mm-3a-core-legacy/"

DEPENDS += "glib-2.0 libxml2 rb-mm-camera-core"

EXTRA_OECONF = "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
               --with-glib \
               --with-common-includes=${STAGING_INCDIR} \
               --with-camera-hal-path=${WORKSPACE}/camera/lib-legacy \
               --with-mm-camera-path=${WORKSPACE}/camera/services/mm-camera-legacy \
               --with-mm-camerasdk-path=${WORKSPACE}/camera/services/mm-camerasdk"

EXTRA_OECONF_append_msm8909 += "TARGET_BOARD_PLATFORM=8909"
EXTRA_OECONF += "CHROMATIX_VERSION=0301"

do_compile_prepend() {

if [ "${BASEMACHINE}" == "msm8909" ]; then
TARGET_PRODUCT=msm8909
fi

LOCAL_CORE_PATH=${S}
TARGET_BUILD_TYPE="release"
TARGET_OUT_HEADERS=${S}
MM_3A_CORE="rb-mm-3a-core"
AU_SEARCH_ON=1

#${S}/version.sh ${LOCAL_CORE_PATH} ${TARGET_PRODUCT} ${TARGET_BUILD_TYPE} ${TARGET_OUT_HEADERS} ${MM_3A_CORE} ${AU_SEARCH_ON}

}

#do_install_append() {
#install -d ${D}${includedir}/rb-mm-3a-core
#install -m 0644 ${S}/rb-mm-3a-core/version.h -D ${D}/usr/include/rb-mm-3a-core
#}

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/* \
                   /lib/firmware/* "
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
INSANE_SKIP_${PN} = "dev-so"
