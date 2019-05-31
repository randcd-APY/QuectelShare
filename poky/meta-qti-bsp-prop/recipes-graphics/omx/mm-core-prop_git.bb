inherit autotools qlicense

DESCRIPTION = "mm-core Proprietary codecs for MSM chipsets"

PR = "r6"

SRC_URI = "file://${WORKSPACE}/mm-core-prop-ship"
S = "${WORKDIR}/mm-core-prop-ship"

DEPENDS = "glib-2.0 virtual/kernel"

PACKAGE_ARCH = "${MACHINE_ARCH}"

EXTRA_OECONF_append = " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                        --with-glib \
                        --enable-target=${BASEMACHINE}"

PACKAGES = "${PN}"
#Disable the split of debug information into -dbg files
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

FILES_${PN} = "${libdir}/*"

# The geofence package contains symlinks that trip up insane
INSANE_SKIP_${PN} = "dev-so"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
