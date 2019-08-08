inherit autotools-brokensep pkgconfig qlicense qprebuilt

DESCRIPTION = "QTI Mobile Connection Manager Core libraries and Service"
PR = "r5"

FILESPATH        =+ "${WORKSPACE}:"
SRC_URI = "file://mcm-core/"
S = "${WORKDIR}/mcm-core/"
SRC_DIR = "${WORKSPACE}/mcm-core/"

DEPENDS = "common-headers qmi qmi-framework mcm xmllib dsutils glib-2.0 data alsa-intf diag"

EXTRA_OECONF = "--with-glib \
                --with-common-includes=${STAGING_INCDIR} \
                --enable-target=${BASEMACHINE}"

FILES_${PN}-dbg += "${prefix}/tests/.debug/"
FILES_${PN}     += "${prefix}/tests/"

do_install_append () {
    install -d ${D}${includedir}
    install -m 555 ${S}/ipc/*.h ${D}${includedir}

    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${S}/mcmril.service.in ${D}${systemd_unitdir}/system/mcmril.service

    install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
    ln -sf ${systemd_unitdir}/system/mcmril.service \
    ${D}${systemd_unitdir}/system/multi-user.target.wants/mcmril.service
}

FILES_${PN} += "/lib/systemd/*"
FILES_${PN} += "/etc/*"
