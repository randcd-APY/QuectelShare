SUMMARY = "Sam System"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

PR = "r2"

FILESPATH_prepend = "${WORKSPACE}/:"
SRC_URI += "file://allplay/sam-system/ \
            file://allplay/OrbProjectFramework/ \
            file://defaults \
            file://system.json \
            file://sam-system.json \
            file://sam-system.service \
            file://wps-supplicant-action \
            file://wps_start \
            file://wps_cancel \
            file://wifi_ant_test \
            file://wifi_ant_read \
            file://readEnetMac \
            file://updateFirmware \
            file://wifi_xput \
            file://enable_mesh.sh \
            file://assign_link_local_ip.sh \
            file://device_speaker_icon.png \
            file://bluetooth.conf \
            file://statemgr \
            file://configureWifiWebOnly \
            "

SRC_DIR = "${WORKSPACE}/allplay/sam-system/"
S = "${WORKDIR}/allplay/sam-system/"

DEPENDS = "boost (>= 1.49) alljoyn alljoyn-services libnl wireless-tools wpa-supplicant-qcacld"
SDEPENDS = "orbutils"
RDEPENDS_${PN} = "alljoyn alljoyn-services libnl wireless-tools wpa-supplicant-qcacld allplay-base curl json-c"

inherit qprebuilt_allplay allplay_scons pkgconfig bluetooth systemd

PACKAGECONFIG ??= "${@bb.utils.contains('DISTRO_FEATURES', 'bluetooth', '${BLUEZ}', '', d)} alljoyn-onboarding 1402-discovery"
PACKAGECONFIG[bluez5] = ",,bluez5"
PACKAGECONFIG[alljoyn-onboarding] = ",,"
PACKAGECONFIG[1402-discovery] = ",,"

CXXFLAGS += "-std=gnu++98"

EXTRA_OESCONS += " \
    ${@bb.utils.contains('PACKAGECONFIG', 'bluez5', 'ENABLE_BLUETOOTH=y', '', d)} \
    ${@bb.utils.contains('PACKAGECONFIG', 'alljoyn-onboarding', 'ENABLE_ALLJOYN_ONBOARDING=y', '', d)} \
    ${@bb.utils.contains('PACKAGECONFIG', '1402-discovery', 'ENABLE_1402_DISCOVERY=y', '', d)} \
    ENABLE_NETWORK_MONITOR=1 \
    "
WEBSERVER_SCRIPT_DIR="/www/websetup/cgi/"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/build/sam-systemd ${D}${bindir}

    install -d ${D}${sysconfdir}/allplay/
    install -m 0644 ${WORKDIR}/system.json ${D}${sysconfdir}/allplay/
    install -m 0644 ${WORKDIR}/sam-system.json ${D}${sysconfdir}/allplay/

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/sam-system.service -D ${D}${systemd_system_unitdir}

    install -d ${D}${base_libdir}/wifi
    install -m 0755 ${WORKDIR}/wps-supplicant-action ${D}${base_libdir}/wifi

    install -d ${D}${sysconfdir}
    install -m 0755 ${WORKDIR}/statemgr ${D}${sysconfdir}

    install -d ${D}${base_sbindir}
    install -m 0755 ${WORKDIR}/wps_start ${D}${base_sbindir}
    install -m 0755 ${WORKDIR}/wps_cancel ${D}${base_sbindir}
    install -m 0755 ${WORKDIR}/wifi_ant_test ${D}${base_sbindir}
    install -m 0755 ${WORKDIR}/wifi_ant_read ${D}${base_sbindir}
    install -m 0755 ${WORKDIR}/readEnetMac ${D}${base_sbindir}
    install -m 0755 ${WORKDIR}/updateFirmware ${D}${base_sbindir}
    install -m 0755 ${WORKDIR}/wifi_xput ${D}${base_sbindir}
    install -m 0755 ${WORKDIR}/enable_mesh.sh ${D}${base_sbindir}
    install -m 0755 ${WORKDIR}/assign_link_local_ip.sh ${D}${base_sbindir}

    install -d ${D}${datadir}/sam-systemd
    install -m 0644 ${WORKDIR}/device_speaker_icon.png ${D}${datadir}/sam-systemd

    if ${@bb.utils.contains('PACKAGECONFIG','bluez5','true','false',d)}; then
        install -d ${D}${sysconfdir}/bluetooth
        install -m 0644 ${WORKDIR}/bluetooth.conf ${D}${sysconfdir}/bluetooth/main.conf
    fi

    install -d ${D}${WEBSERVER_SCRIPT_DIR}
    install -m 0755 ${WORKDIR}/configureWifiWebOnly  ${D}${WEBSERVER_SCRIPT_DIR}/
}

FILES_${PN} += " \
	${datadir}/sam-systemd \
	${base_libdir}/wifi \
	${systemd_system_unitdir} \
	${sysconfdir}/allplay/ \
    ${WEBSERVER_SCRIPT_DIR} \
	"

SYSTEMD_SERVICE_${PN} = "sam-system.service"
