inherit qcommon qlicense qprebuilt

DESCRIPTION = "libaudioalsa Library"
PR = "r5"

DEPENDS = "glib-2.0 virtual/kernel"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/audio-alsa/"
S = "${WORKDIR}/audio/mm-audio/audio-alsa/"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

EXTRA_OECONF += "\
                 --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                 --with-glib"
