inherit autotools qcommon qlicense


DESCRIPTION = "quartz library & applications"

PR = "r0"
PV = "1.0"

SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/qzhost/"
S = "${WORKDIR}/vendor/qcom/proprietary/qzhost/quartz-host"

SRC_URI += "file://iotd_config.ini"
SRC_URI += "file://0000-platform.patch"

TARGET_CC_ARCH += "${LDFLAGS}"

do_patch() {
    if [ -f ${WORKDIR}/0000-platform.patch ]; then
        cd ${S}
#       patch -p2 < ${WORKDIR}/0000-platform.patch
    fi;
}

do_compile() {
    make CC="${CC}" HOST_CROSS_COMPILE=other -C exthost/Linux/qapi
    make CC="${CC}" HOST_CROSS_COMPILE=other -C exthost/Linux/daemon
    make CC="${CC}" HOST_CROSS_COMPILE=other -C exthost/Linux/app/NB_QCLI_demo/build
    make CC="${CC}" HOST_CROSS_COMPILE=other -C exthost/Linux/app/fwupgrade
}

do_install() {

    install -m 0644 ${WORKDIR}/iotd_config.ini -D ${D}${sysconfdir}/iotd_config.ini

    install -d ${D}${includedir}/quartz_host/

    install -d ${D}${bindir}
    install -m 0755 exthost/Linux/app/NB_QCLI_demo/build/nb_demo -D ${D}${bindir}/
    install -m 0755 exthost/Linux/daemon/output/iotd -D ${D}${bindir}
    install -m 0755 exthost/Linux/app/fwupgrade/qca_mgr_daemon -D ${D}${bindir}
}

FILES_${PN} = "/usr/bin/"
FILES_${PN} += "${sysconfdir}/"
