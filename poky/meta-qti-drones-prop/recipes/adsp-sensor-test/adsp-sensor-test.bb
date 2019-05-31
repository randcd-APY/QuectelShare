inherit autotools qcommon qlicense qprebuilt
DESCRIPTION = "adsp-sensor-test"

PR = "r0"
PV = "1.0"

DEPENDS += "glib-2.0 diag libxml2"
INSANE_SKIP_${PN} += "dev-deps"

S = "${WORKDIR}/vendor/qcom/proprietary/drones/sensortest"
SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/drones/sensortest/"
SRC_URI += "file://adsp_sensor_test.service"

EXTRA_OECONF += "--with-glib"
FILES_${PN} += "${systemd_unitdir}/system/"

do_install_append() {
    dest=/etc/mmi
    install -m 0755 -d ${D}${dest}
    install -m 0755 ${S}/config/sensor.config -D ${D}${dest}/sensor.config
    install -m 0755 ${S}/config/start_adsp_sensor_test -D ${D}${sysconfdir}/init.d/start_adsp_sensor_test
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_unitdir}/system/
        install -m 0644 ${WORKDIR}/adsp_sensor_test.service -D ${D}${systemd_unitdir}/system/adsp_sensor_test.service
        install -d ${D}${systemd_unitdir}/system/ffbm.target.wants/
        # enable the service for ffbm.target
        ln -sf ${systemd_unitdir}/system/adsp_sensor_test.service \
            ${D}${systemd_unitdir}/system/ffbm.target.wants/adsp_sensor_test.service
    fi

}

pkg_postinst_${PN} () {
    [ -n "$D" ] && OPT="-r $D" || OPT="-s"
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'false', 'true', d)}; then
            update-rc.d $OPT -f start_adsp_sensor_test remove
            update-rc.d $OPT start_adsp_sensor_test start 97 4 .
    fi
}
