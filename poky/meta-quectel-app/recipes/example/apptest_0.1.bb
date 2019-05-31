#
# This file was derived from the 'Hello World!' example recipe in the
# Yocto Project Development Manual.
#

inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel App Test"


DEPENDS = "glib-2.0"

SRC_DIR = "${WORKSPACE}/quectel-app/example/apptest/"
S = "${WORKDIR}/quectel-app/example/apptest"


