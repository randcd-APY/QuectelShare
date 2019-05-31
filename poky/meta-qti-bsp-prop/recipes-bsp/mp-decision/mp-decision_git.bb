inherit autotools qlicense

DESCRIPTION = "MP Decision library for MSM/QSD"

PR = "r7"
SRC_URI = "file://${WORKSPACE}/mp-decision"

S = "${WORKDIR}/${PN}"

DEPENDS += "perf-libs"
DEPENDS += "dlog"

PACKAGE_ARCH = "${MACHINE_ARCH}"

EXTRA_OECONF_append = "--enable-target-${BASEMACHINE}=yes"
EXTRA_OECONF_append = " --with-dlog"

FILES_${PN} += "\
    /usr/lib/* \
    /usr/bin/* \
"

# The mpdecision package contains symlinks that trip up insane
INSANE_SKIP_${PN} = "dev-so"

