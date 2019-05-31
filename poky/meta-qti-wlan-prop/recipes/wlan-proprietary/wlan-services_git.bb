inherit autotools qcommon qlicense update-rc.d qprebuilt

DESCRIPTION = "WLAN Services required for QCACLD"
PR = "r2"

DEPENDS = "data qcacld-ll"
DEPENDS_mdm9607 = "data qcacld-hl"
DEPENDS_msm8909 = "data qcacld-hl"
DEPENDS_apq8017 = "data qcacld-hl"
DEPENDS_sdx20 = "data qcacld30-ll"
DEPENDS_sdxpoorwills = "data qcacld30-ll"
DEPENDS_sdxprairie = "data qcacld32-ll"

RDEPENDS_${PN} = "data"

SRC_DIR = "${WORKSPACE}/wlan-proprietary/wlan-services/"

S = "${WORKDIR}/wlan-proprietary/wlan-services/qcacld"

EXTRA_OECONF = "--enable-debug"

INITSCRIPT_NAME = "start_wlan_services"
INITSCRIPT_PARAMS = "start 90 2 3 4 5 . stop 10 0 1 6 ."

SRC_URI +="file://start_wlan_services.service"

do_install_append () {

    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
     install -m 0755 ${WORKDIR}/wlan-proprietary/wlan-services/qcacld/start_wlan_services -D ${D}${sysconfdir}/initscripts/start_wlan_services
     install -d ${D}/etc/systemd/system/
     install -m 0644 ${WORKDIR}/start_wlan_services.service -D ${D}/etc/systemd/system/start_wlan_services.service
     install -d ${D}/etc/systemd/system/multi-user.target.wants/
     ln -sf /etc/systemd/system/start_wlan_services.services \
                                      ${D}/etc/systemd/system/multi-user.target.wants/start_wlan_services.service

    fi
}
