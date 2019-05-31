inherit qcommon qlicense qprebuilt

DESCRIPTION = "acdb-fts Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-cal/audio-acdb-util/acdb-fts/"
S = "${WORKDIR}/audio/mm-audio-cal/audio-acdb-util/acdb-fts"

DEPENDS = "audcal libcutils system-core"

do_install_append () {
  cp ${D}/${libdir}/libacdb_fts.so ${D}/${libdir}/libacdb-fts.so
}

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
