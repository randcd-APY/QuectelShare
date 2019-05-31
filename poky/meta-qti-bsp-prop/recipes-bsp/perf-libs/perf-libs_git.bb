inherit autotools qlicense

DESCRIPTION = "mp-ctl and libqc-opt library to add all perf optimizations"
PR = "r3"

SRC_URI = "file://${WORKSPACE}/perf-libs"
S = "${WORKDIR}/perf-libs"

DEPENDS += "system-core"
DEPENDS += "glib-2.0"
DEPENDS += "icu"
DEPENDS += "dlog"

EXTRA_OECONF_append = " --with-glib"
EXTRA_OECONF_append = " --with-dlog"
EXTRA_OECONF_append = " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF_append = " --with-mpdecision-includes=${WORKSPACE}/mp-decision"
EXTRA_OECONF_append = " --with-additional-includes=${WORKSPACE}/device/qcom/common/power"
EXTRA_OECONF_append = " --enable-target=${BASEMACHINE}"

LDFLAGS += "-Wl,--build-id=sha1"

#Disable the split of debug information into -dbg files
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

FILES_${PN} = "${libdir}/*"

# The package contains symlinks that trip up insane
INSANE_SKIP_${PN} = "dev-so"

CXXFLAGS     += "-I${STAGING_INCDIR}/c++"
CXXFLAGS     += "-I${STAGING_INCDIR}/c++/${TARGET_SYS}"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
