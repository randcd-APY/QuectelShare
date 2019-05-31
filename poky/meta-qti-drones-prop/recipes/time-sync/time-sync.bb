DESCRIPTION = "time_sync server module"

inherit autotools qcommon qlicense systemd

SRC_URI = "file://time_sync.sh \
           file://time_sync.service"

do_install() {
    install -d ${D}/${sysconfdir}/init.d

    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -m 755 ${WORKDIR}/time_sync.sh -D ${D}/${sysconfdir}/time_sync.sh
        install -d ${D}${systemd_unitdir}/system/
        install -m 0644 ${WORKDIR}/time_sync.service -D ${D}${systemd_unitdir}/system/time_sync.service
        install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
        ln -sf ${systemd_unitdir}/system/time_sync.service \
           ${D}${systemd_unitdir}/system/multi-user.target.wants/time_sync.service
    else
        install -m 755 ${WORKDIR}/time_sync.sh -D ${D}/${sysconfdir}/init.d/time_sync.sh
    fi
}

pkg_postinst_${PN} () {
         [ -n "$D" ] && OPT="-r $D" || OPT="-s"
         if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'false', 'true', d)}; then
             update-rc.d $OPT -f time_sync remove
             update-rc.d $OPT time_sync start 4 2 3 5 .
         fi
}

FILES_${PN} += "${systemd_unitdir}/system/"
