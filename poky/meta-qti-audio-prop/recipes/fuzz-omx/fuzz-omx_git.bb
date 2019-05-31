inherit qcommon qlicense qprebuilt

DESCRIPTION = "fuzz-omx Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-noship/fuzz-omx/"
S = "${WORKDIR}/audio/mm-audio-noship/fuzz-omx/"

DEPENDS = "libcutils media glib-2.0"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF += "--with-glib"
EXTRA_OEMAKE += "DEFAULT_INCLUDES=-I${WORKSPACE}/hardware/qcom/media/mm-core/inc"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
