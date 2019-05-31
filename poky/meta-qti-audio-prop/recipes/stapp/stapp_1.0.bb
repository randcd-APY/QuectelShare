inherit qcommon qlicense

DESCRIPTION = "soundtrigger test app"
PR = "r1"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-apps/stapp/"

S = "${WORKDIR}/audio/mm-audio-apps/stapp/"

DEPENDS = "libhardware soundtrigger"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF += "--with-libhardware-includes=${STAGING_INCDIR}"
EXTRA_OECONF += "--with-strig-includes=${STAGING_INCDIR}/sound_trigger"
