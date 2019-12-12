#
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel App Test"


DEPENDS = "glib-2.0 ql-bt"

SRC_DIR = "${WORKSPACE}/quectel-app/bt/"
S = "${WORKDIR}/quectel-app/bt"

PACKAGES = "${PN}"
FILES_${PN} += "${libdir}/*"
FILES_${PN} += "/usr/include/*"
INSANE_SKIP_${PN} = "dev-deps"


