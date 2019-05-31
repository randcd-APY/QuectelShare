inherit autotools pkgconfig qlicense

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}/camera/:"
SRC_URI   = "file://lib-legacy"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/lib-legacy"

SRC_DIR = "${WORKSPACE}/camera/lib-legacy"

DEPENDS = "media av-frameworks glib-2.0 display-hal-linux"

LDFLAGS_append_quec-smart += "-lm"
CPPFLAGS_append_quec-smart += "-I${WORKSPACE}/frameworks/native/include/"
CPPFLAGS_append_quec-smart += "-I${WORKSPACE}/hardware/qcom/media/"

EXTRA_OECONF = "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF += "--with-glib"
EXTRA_OECONF += "--with-common-includes=${STAGING_INCDIR}"

include rb-camera-msm8909.inc

FILES_${PN}-dbg  = "${libdir}/.debug/* /usr/bin/.debug/* /usr/lib/hw/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/* ${libdir}/hw/*.so"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir} ${STAGING_DIR_HOST}/lib/*"
INSANE_SKIP_${PN} = "dev-so"
