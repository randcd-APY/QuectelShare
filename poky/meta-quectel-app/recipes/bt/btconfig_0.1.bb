#
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel App Test"


DEPENDS = "glib-2.0"

SRC_DIR = "${WORKSPACE}/quectel-app/btconfig/"
S = "${WORKDIR}/quectel-app/btconfig"

PACKAGES = "${PN}"
FILES_${PN} += "${libdir}/*"
FILES_${PN} += "/usr/include/*"
INSANE_SKIP_${PN} = "dev-deps"


