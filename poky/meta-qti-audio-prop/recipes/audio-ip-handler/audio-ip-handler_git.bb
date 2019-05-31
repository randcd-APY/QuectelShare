inherit qcommon qlicense qprebuilt

DESCRIPTION = "audio_ip_handler"
SECTION = "multimedia"
PR = "r0"

DEPENDS = "glib-2.0 libcutils system-media audio-qaf virtual/kernel"

EXTRA_OEMAKE = "DEFAULT_INCLUDES= CPPFLAGS="-I. -I${STAGING_KERNEL_BUILDDIR}/usr/include""

SRC_DIR = "${WORKSPACE}/audio/mm-audio-noship/audio-ip-handler/"
S = "${WORKDIR}/audio/mm-audio-noship/audio-ip-handler/"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

EXTRA_OECONF = "--with-glib"
EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"

FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/*.so"
