inherit qcommon qlicense update-rc.d qprebuilt
DESCRIPTION = "Daemon to perform scrubbing"

FILESPATH =+ "${WORKSPACE}:"
SRC_DIR = "${WORKSPACE}/fs-scrub-daemon/"
S = "${WORKDIR}/fs-scrub-daemon/"

DEPENDS += "glib-2.0"
DEPENDS += "qmi-framework"

EXTRA_OECONF_append = " --with-glib"

PR = "r1"

do_install() {
        install -m 0755 ${S}/fs-scrub-daemon -D ${D}/sbin/fs-scrub-daemon
        install -m 0755 ${WORKDIR}/fs-scrub-daemon/start_fs-scrub-daemon -D ${D}${sysconfdir}/init.d/start_fs-scrub-daemon
}

inherit update-rc.d

INITSCRIPT_NAME = "start_fs-scrub-daemon"
INITSCRIPT_PARAMS = "start 99 2 3 4 5 . stop 1 0 1 6 ."

pkg_postinst_${PN} () {
        [ -n "$D" ] && OPT="-r $D" || OPT="-s"
        # remove all rc.d-links potentially created from alternatives
        update-rc.d $OPT -f ${INITSCRIPT_NAME} remove
        update-rc.d $OPT ${INITSCRIPT_NAME} ${INITSCRIPT_PARAMS}
}
