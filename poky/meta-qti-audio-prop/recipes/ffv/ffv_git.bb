inherit qcommon qprebuilt qlicense

DESCRIPTION = "ffv Library"
PR = "r0"

do_fetch_extra () {
  cp -rf ${WORKSPACE}/poky/meta-qti-audio-prop/recipes/ffv/config/ ${WORKDIR}
}
addtask do_fetch_extra before do_fetch

EXTRA_OECONF_append_msm8909 = " BOARD_SUPPORTS_FFV_EC_THREAD_RT_PRIORITY=true"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-noship/ffv/"
S = "${WORKDIR}/audio/mm-audio-noship/ffv/"

DEPENDS = "virtual/kernel"
DEPENDS += "lms libcutils"

SOLIBS = ".so"
FILES_SOLIBSDEV = ""

do_install_append () {
  if [ -d "${WORKDIR}/config/${BASEMACHINE}" ] && [ $(ls -1 ${WORKDIR}/config/${BASEMACHINE} | wc -l) -ne 0 ]; then
    install -d ${D}${sysconfdir}
    install -m 0444 ${WORKDIR}/config/${BASEMACHINE}/* ${D}${sysconfdir}/
  fi
}
