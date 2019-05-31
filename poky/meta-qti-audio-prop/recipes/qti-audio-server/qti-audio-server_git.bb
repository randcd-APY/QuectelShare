inherit autotools qcommon qlicense

DESCRIPTION = "qti-audio-server Library"
PR = "r0"

S = "${WORKDIR}/audio/mm-audio/qti-audio-server/"

DEPENDS = "tinyalsa libcutils libhardware binder qahw qsthw"

EXTRA_OECONF_append_apq8017 = " BOARD_SUPPORTS_QTI_AUDIO_SERVER=true"
EXTRA_OECONF_append_msm8909 = " BOARD_SUPPORTS_QTI_AUDIO_SERVER=true"
EXTRA_OECONF_append_apq8053 = " BOARD_SUPPORTS_QTI_AUDIO_SERVER=true"
EXTRA_OEMAKE = "DEFAULT_INCLUDES="-I${STAGING_INCDIR}/qti_audio_server""

SOLIBS = ".so"
FILES_SOLIBSDEV = ""

FILESPATH =+ "${WORKSPACE}:"
SRC_URI  = "file://audio/mm-audio/qti-audio-server/"
SRC_URI += "file://qti_audio_server_daemon.sh"
SRC_URI += "file://qti_audio_server_daemon.service"

do_install_append() {
   if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
       install -d ${D}${systemd_unitdir}/system/
       install -m 0644 ${WORKDIR}/qti_audio_server_daemon.service -D ${D}${systemd_unitdir}/system/qti_audio_server_daemon.service
       install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/

       # enable the service for multi-user.target
       ln -sf ${systemd_unitdir}/system/qti_audio_server_daemon.service \
            ${D}${systemd_unitdir}/system/multi-user.target.wants/qti_audio_server_daemon.service
   fi
}

FILES_${PN} += "${systemd_unitdir}/system/"
