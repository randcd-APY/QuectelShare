inherit qcommon qlicense qprebuilt

DESCRIPTION = "audio-dev-arbi Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/audio-dev-arbi/"
S = "${WORKDIR}/audio/mm-audio/audio-dev-arbi"

DEPENDS = "libhardware libcutils glib-2.0"

EXTRA_OECONF_append = "--with-glib"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
