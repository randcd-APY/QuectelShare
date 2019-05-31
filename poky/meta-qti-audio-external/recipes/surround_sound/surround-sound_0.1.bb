inherit qcommon qlicense qprebuilt

DESCRIPTION = "surround_sound library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-external/audio-systems/surround_sound/"
S = "${WORKDIR}/audio/mm-audio-external/audio-systems/surround_sound/"

do_install_append () {
  install -d ${D}${sysconfdir}/surround_sound
  install -m 0755 ${S}/filters/* ${D}${sysconfdir}/surround_sound/
}
