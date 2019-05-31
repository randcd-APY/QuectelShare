inherit qcommon qlicense qprebuilt

DESCRIPTION = "m8"
SECTION = "multimedia"
PR = "r0"

DEPENDS = "glib-2.0 libcutils system-media acdbloader tinycompress tinyalsa audio-qaf audio-qap-wrapper"

EXTRA_OEMAKE = "DEFAULT_INCLUDES= CPPFLAGS="-I. -I${STAGING_KERNEL_BUILDDIR}/usr/include""

SRC_DIR = "${WORKSPACE}/audio/mm-audio-external-noship/audio-3rdparty/dts/m8/"
S = "${WORKDIR}/audio/mm-audio-external-noship/audio-3rdparty/dts/m8/"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

EXTRA_OECONF = "--with-glib"
EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF += "AUDIO_FEATURE_ENABLE_DTS_M6=true"

FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/*.so"

ALLOW_EMPTY_${PN} = "1"
