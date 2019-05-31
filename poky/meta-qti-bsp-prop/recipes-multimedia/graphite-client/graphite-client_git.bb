inherit qcommon qprebuilt qlicense

DESCRIPTION = "Graphite-client Library"
PR = "r0"

DEPENDS = "liblog expat"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/graphite-client/"
S = "${WORKDIR}/audio/mm-audio/graphite-client/"

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
