inherit qcommon qlicense qprebuilt

DESCRIPTION = "m8"
SECTION = "multimedia"
PR = "r0"

M8BINSUFFIX = "${@base_contains('TUNE_ARCH', 'aarch64', '_64bit', '', d)}"
DEPENDS = "glib-2.0 libcutils system-media acdbloader tinycompress tinyalsa audio-qaf audio-qap-wrapper audio-dts-m8-external-noship"

EXTRA_OEMAKE = "DEFAULT_INCLUDES= CPPFLAGS="-I. -I${STAGING_KERNEL_BUILDDIR}/usr/include""

SRC_DIR = "${WORKSPACE}/audio/mm-audio-noship/dts/m8/"
S = "${WORKDIR}/audio/mm-audio-noship/dts/m8/"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

EXTRA_OECONF = "--with-glib"
EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF += "AUDIO_FEATURE_ENABLE_DTS_M6=true"
EXTRA_OECONF += "--with-glib --program-suffix=${M8BINSUFFIX}"
EXTRA_OECONF_append_apq8098 = " AUDIO_FEATURE_ENABLED_QAP=true"

FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/*.so"
