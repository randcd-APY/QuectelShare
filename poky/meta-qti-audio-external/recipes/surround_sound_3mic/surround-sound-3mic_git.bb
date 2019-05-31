inherit qcommon qprebuilt qlicense

DESCRIPTION = "surround_sound_3mic Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-external-noship/audio-systems/surround_sound_3mic/"
S = "${WORKDIR}/audio/mm-audio-external-noship/audio-systems/surround_sound_3mic/"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"

do_install_append () {
  install -d ${D}${sysconfdir}
  install -m 0755 ${S}/cfg/default/* ${D}${sysconfdir}/
}

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
