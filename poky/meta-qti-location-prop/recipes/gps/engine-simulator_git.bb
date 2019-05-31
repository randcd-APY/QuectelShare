inherit autotools qcommon qlicense qprebuilt

DESCRIPTION = "Engine Plugin api libary"
PR = "r1"

SRC_DIR = "${WORKSPACE}/gps-noship/engine-plugin/simulator/"
S = "${WORKDIR}/gps-noship/engine-plugin/simulator"
DEPENDS = "glib-2.0 gps-utils loc-base-util engine-plugin-api-hdr engine-plugin-impl-hdr"
EXTRA_OECONF ="--with-glib"
PACKAGES = "${PN}"
FILES_${PN} = "${libdir}/*"
FILES_${PN} += "/usr/include/*"
INSANE_SKIP_${PN} = "dev-so"
