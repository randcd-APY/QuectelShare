inherit autotools-brokensep pkgconfig  update-rc.d qlicense qprebuilt

DESCRIPTION = "Persistent device config Daemon"
PR = "r1"

FILESPATH        =+ "${WORKSPACE}:"
SRC_URI = "file://mdm-ss-mgr/pdc-daemon/"
S = "${WORKDIR}/mdm-ss-mgr/pdc-daemon/"
SRC_DIR = "${WORKSPACE}/mdm-ss-mgr/pdc-daemon/"

DEPENDS = "glib-2.0"
DEPENDS += "qmi-framework"

INITSCRIPT_NAME = "start_pdcd"
INITSCRIPT_PARAMS = "start 99 2 3 4 5 ."

do_install_append() {
       install -m 0755 ${S}/start_pdcd -D ${D}${sysconfdir}/init.d/start_pdcd
}

