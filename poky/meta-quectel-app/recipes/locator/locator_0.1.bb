#
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel App Test"


DEPENDS = "glib-2.0 ql-common-api ql-mcm-api ql-manager"

SRC_DIR = "${WORKSPACE}/quectel-app/locator/"
S = "${WORKDIR}/quectel-app/locator"

PACKAGES = "${PN}"
FILES_${PN} += "${libdir}/*"
FILES_${PN} += "/usr/include/*"
INSANE_SKIP_${PN} = "dev-deps"


