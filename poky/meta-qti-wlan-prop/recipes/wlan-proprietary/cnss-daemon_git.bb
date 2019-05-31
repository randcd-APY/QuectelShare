inherit autotools qcommon qlicense update-rc.d qprebuilt

DESCRIPTION = "CNSS"
PR = "r2"

DEPENDS = "qmi qmi-framework libcutils libnl"

FILESPATH =+ "${WORKSPACE}/wlan-proprietary/:"

SRC_URI = "file://cnss-daemon/"

SRC_DIR = "${WORKSPACE}/wlan-proprietary/cnss-daemon"

S = "${WORKDIR}/cnss-daemon"

CFLAGS += "-I ${STAGING_INCDIR}/libnl3"
CFLAGS += "-I ${WORKSPACE}/system/core/include/"

EXTRA_OECONF = "--enable-debug"

INITSCRIPT_NAME = "start_cnss_daemon"
INITSCRIPT_PARAMS = "start 90 2 3 4 5 . stop 10 0 1 6 ."

SRC_URI +="file://start_cnss_daemon.service"

do_install_append () {
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
    install -m 0755 ${S}/cnss_daemon -D ${D}/usr/sbin/cnss_daemon
    install -d ${D}/etc/systemd/system/
    install -m 0644 ${WORKDIR}/start_cnss_daemon.service -D ${D}/etc/systemd/system/start_cnss_daemon.service
    install -d ${D}/etc/systemd/system/multi-user.target.wants/
    ln -sf /etc/systemd/system/start_cnss_daemon.service \
        ${D}/etc/systemd/system/multi-user.target.wants/start_cnss_daemon.service
    fi
}
