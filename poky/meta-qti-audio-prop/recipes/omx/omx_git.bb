inherit qcommon qprebuilt qlicense

DESCRIPTION = "omx Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/omx/"
S = "${WORKDIR}/audio/mm-audio/omx/"

DEPENDS = "media audioalsa glib-2.0 omx-noship"
DEPENDS_append = "${@bb.utils.contains('DISTRO_FEATURES', 'audio-dlkm', '', '', d)}"

RDEPENDS_${PN} = "media"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                 --with-glib \
                 --with-feature-omx-adec-g711"
EXTRA_OECONF += "PLATFORM_BOARD=${BASEMACHINE}"

EXTRA_OEMAKE += "DEFAULT_INCLUDES=-I${WORKSPACE}/hardware/qcom/media/mm-core/inc"

CPPFLAGS_apq8010 += "-DQCOM_AUDIO_USE_SYSTEM_HEAP_ID"
CPPFLAGS_apq8037 += "-DQCOM_AUDIO_USE_SYSTEM_HEAP_ID"
CPPFLAGS_apq8052 += "-DQCOM_AUDIO_USE_SYSTEM_HEAP_ID"
CPPFLAGS_apq8053 += "-DQCOM_AUDIO_USE_SYSTEM_HEAP_ID"
CPPFLAGS_apq8084 += "-DQCOM_AUDIO_USE_SYSTEM_HEAP_ID"
CPPFLAGS_apq8096 += "-DQCOM_AUDIO_USE_SYSTEM_HEAP_ID"
CPPFLAGS_apq8017 += "-DQCOM_AUDIO_USE_SYSTEM_HEAP_ID"

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
