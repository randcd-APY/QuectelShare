inherit autotools qcommon qlicense qprebuilt

DESCRIPTION = "Display NoShip"
PR = "r3"

PACKAGES = "${PN}"

SRC_DIR = "${WORKSPACE}/display/display-noship/"
S = "${WORKDIR}/display/display-noship/"

PREBUILT = "1"

DEPENDS += "display-hal"

EXTRA_OECONF = " --with-core-includes=${WORKSPACE}/system/core/include"
EXTRA_OECONF += " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"

LDFLAGS += "-llog -lutils -lcutils -lsdmutils"
CPPFLAGS += "-DTARGET_HEADLESS"

CPPFLAGS += "-I${WORKSPACE}/display/display-hal/include"
CPPFLAGS += "-I${WORKSPACE}/display/display-noship/hdr_tm"

# Need the display sdm headers
CPPFLAGS += "-I${STAGING_INCDIR}/sdm"

FILES_${PN} = "${libdir}/*.so"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
