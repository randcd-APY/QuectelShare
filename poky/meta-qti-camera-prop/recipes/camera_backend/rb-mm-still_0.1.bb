inherit autotools pkgconfig qlicense qprebuilt

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/services/mm-still-legacy"
SRC_DIR = "${WORKSPACE}/camera/services/mm-still-legacy"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/camera/services/mm-still-legacy"

DEPENDS += "glib-2.0 libxml2 rb-mm-camera rb-camera adsprpc"

EXTRA_OECONF = "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                --with-glib \
                --with-common-includes=${STAGING_INCDIR} \
                --with-camera-hal-path=${WORKSPACE}/camera/lib-legacy \
                --with-mm-camera-path=${WORKSPACE}/camera/services/mm-camera-legacy/mm-camera2 \
                --with-mm-camerasdk=${WORKSPACE}/camera/services/mm-camerasdk"

EXTRA_OECONF_append_msm8909 += "TARGET_BOARD_PLATFORM=8909 MSM_VERSION=8909"
EXTRA_OECONF += "TARGET_USES_ION=true"
EXTRA_OECONF += "CHROMATIX_VERSION=0301"
EXTRA_OECONF += "NEW_LOG_API=true"
EXTRA_OECONF += "JPEC_ENC=hw_sw"
EXTRA_OECONF += "USES_ARMV7=true"
EXTRA_OECONF += "FACT_VER=codecB"
EXTRA_OECONF += "CODECV1_LIST=true"

EXTRA_OECONF += "JPEGD_VER=jpeg10"
EXTRA_OECONF += "JPEGE_VER=jpeg10"
EXTRA_OECONF += "FACT_VER=codecB"
EXTRA_OECONF += "USES_GEMINI=false"
EXTRA_OECONF += "USES_MERCURY=false"
EXTRA_OECONF += "SMIPOOL_AVAILABLE=false"
EXTRA_OECONF += "MM_STILL_OMX_COMP=false"
EXTRA_OECONF += "MM_STILL_OMX_FUZZ=false"
EXTRA_OECONF_append_quec-smart += "JPEG_USE_FASTCV_OPT=false"

do_compile_prepend() {
    make clean
}

do_install_append() {
  mkdir -p ${D}/system/bin/
#cp ${D}${bindir}/* ${D}/system/bin/

  mv ${D}/usr/include/mm-still-legacy ${D}/usr/include/mm-still
}


PACKAGES = "${PN}"
FILES_${PN}-dbg  = "/usr/lib/.debug/* /system/bin/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/*"
FILES_${PN}-dev  = "/usr/lib/*.la ${includedir}"
INSANE_SKIP_${PN} = "dev-so "
INSANE_SKIP_${PN} += "staticdev"
INSANE_SKIP_${PN} += "installed-vs-shipped"
