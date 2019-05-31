inherit qcommon qprebuilt qlicense

DESCRIPTION = "audio-resampler Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-external-noship/audio-systems/audio-resampler/"
S = "${WORKDIR}/audio/mm-audio-external-noship/audio-systems/audio-resampler/"

DEPENDS = "liblog"

do_install_append () {
  cp ${D}/${libdir}/libmm_audio_resampler.so ${D}/${libdir}/libmm-audio-resampler.so
}

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
