inherit qcommon qprebuilt qlicense

DESCRIPTION = "audio-listen Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-noship/audio-listen/"
S = "${WORKDIR}/audio/mm-audio-noship/audio-listen/"

DEPENDS = "libcutils glib-2.0"

EXTRA_OECONF += "--with-glib"
EXTRA_OECONF += "BUILD_ARM_LSMLIB=false"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
