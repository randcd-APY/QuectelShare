inherit qcommon qlicense qprebuilt

DESCRIPTION = "qap"
SECTION = "multimedia"
PR = "r0"

DEPENDS = "glib-2.0 libcutils system-media audio-qaf audio-ip-handler"

EXTRA_OEMAKE = "DEFAULT_INCLUDES= CPPFLAGS="-I. -I${STAGING_KERNEL_BUILDDIR}/usr/include""

SRC_DIR = "${WORKSPACE}/audio/mm-audio/qap_wrapper/"
S = "${WORKDIR}/audio/mm-audio/qap_wrapper/"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

QAPBINSUFFIX = "${@base_contains('TUNE_ARCH', 'aarch64', '_64bit', '', d)}"
EXTRA_OECONF = "--with-glib --program-suffix=${QAPBINSUFFIX}"
EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"

FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/*.so"
