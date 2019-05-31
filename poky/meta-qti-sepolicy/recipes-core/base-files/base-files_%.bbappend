FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append += " \
    file://systemd/label-cache.service \
    file://systemd/label-persist.service \
    file://systemd/label-systemrw.service \
    file://systemd/label-data.service \
"

do_install_append() {
    if ${@base_contains('DISTRO_FEATURES','selinux','true','false',d)}; then
        if ${@base_contains('DISTRO_FEATURES','systemd','true','false',d)}; then
            if ${@base_contains('DISTRO_FEATURES','nand-boot','false','true',d)}; then
                install -m 0644 ${WORKDIR}/systemd/label-cache.service ${D}${sysconfdir}/systemd/system/label-cache.service
                install -m 0644 ${WORKDIR}/systemd/label-persist.service ${D}${sysconfdir}/systemd/system/label-persist.service
                install -m 0644 ${WORKDIR}/systemd/label-systemrw.service ${D}${sysconfdir}/systemd/system/label-systemrw.service
                install -m 0644 ${WORKDIR}/systemd/label-data.service ${D}${sysconfdir}/systemd/system/label-data.service

                ln -sf  ../label-cache.service  ${D}${sysconfdir}/systemd/system/local-fs.target.requires/label-cache.service
                ln -sf  ../label-persist.service  ${D}${sysconfdir}/systemd/system/local-fs.target.requires/label-persist.service
                ln -sf  ../label-systemrw.service  ${D}${sysconfdir}/systemd/system/local-fs.target.requires/label-systemrw.service
                ln -sf  ../label-data.service  ${D}${sysconfdir}/systemd/system/local-fs.target.requires/label-data.service
            fi
        fi
    fi
}
