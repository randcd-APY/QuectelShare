inherit autotools qcommon qlicense qprebuilt
DESCRIPTION = "WAP FOTA DAEMON"
PR = "r3"
DEPENDS = "qmi-framework glib-2.0"

SRC_DIR = "${WORKSPACE}/wap-fota-daemon"
S = "${WORKDIR}/wap-fota-daemon"

FILES_${PN}-dbg += "${sysconfdir}/init.d/.debug"

EXTRA_OECONF += "--with-glib --with-common-includes=${STAGING_INCDIR}"

do_install_append() {
        install -m 0755 ${WORKDIR}/wap-fota-daemon/start_wapfotadaemon -D ${D}${sysconfdir}/init.d/wapfotadaemon
}
