inherit autotools qcommon qprebuilt qlicense update-rc.d

DESCRIPTION = "energy-awareness"

PR = "r0"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://core-technologies/energy-awareness/"

SRC_DIR = "${WORKSPACE}/core-technologies/energy-awareness/"
S = "${WORKDIR}/core-technologies/energy-awareness/"

DEPENDS += "glib-2.0 libcutils virtual/kernel"

EXTRA_OECONF += " --with-glib"
EXTRA_OECONF += " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF += " --enable-target-${BASEMACHINE}=yes"

INITSCRIPT_NAME = "energy-awareness"
INITSCRIPT_PARAMS = "start 40 2 3 4 5 ."

FILES_${PN} += "\
       /usr/bin/*"

do_install_append() {
       install -m 0755 ${S}/start_energy-awareness_le -D ${D}${sysconfdir}/init.d/energy-awareness
}
do_configure[depends] += "virtual/kernel:do_shared_workdir"
