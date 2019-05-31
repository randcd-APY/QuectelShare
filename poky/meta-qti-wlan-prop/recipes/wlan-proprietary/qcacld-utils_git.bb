inherit qcommon qprebuilt

DESCRIPTION = "Qualcomm Atheros WLAN CLD utils"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/\
${LICENSE};md5=92b1d0ceea78229551577d4284669bb8"

PR = "r1"

DEPENDS = "diag libcutils"
DEPENDS_append_msm8909 = " libnl"
DEPENDS_append_sa8155 = " libnl"
DEPENDS_append_sa8155qdrive = " libnl"
DEPENDS_append_mdm9650 = " dsutils qmi qmi-framework wpa-supplicant-qcacld libnl qcacld-ll"
DEPENDS_append_sdx20 = " dsutils qmi qmi-framework wpa-supplicant-qcacld libnl qcacld30-ll"
DEPENDS_append_qcs605 = " dsutils qmi qmi-framework wpa-supplicant-qcacld libnl qcacld30-ll"
DEPENDS_append_sdxpoorwills = " dsutils qmi qmi-framework wpa-supplicant-qcacld libnl qcacld30-ll"
DEPENDS_append_qcs40x = " dsutils qmi qmi-framework wpa-supplicant-qcacld libnl qcacld32-ll"
DEPENDS_append_sdxprairie = " dsutils qmi qmi-framework wpa-supplicant-qcacld libnl qcacld32-ll"

RDEPENDS_${PN}_mdm9650 = "wpa-supplicant-qcacld"
RDEPENDS_${PN}_sdx20 = "wpa-supplicant-qcacld"
RDEPENDS_${PN}_sdxpoorwills = "wpa-supplicant-qcacld"
FILES_${PN} += "${sysconfdir}/misc/wifi/*"

EXTRA_OECONF = "--with-glib \
                --enable-target=${BASEMACHINE}"
EXTRA_OECONF_append_mdm9650 = " --enable-target-mdm9650=yes --with-qmi-cli --with-qxdm"
EXTRA_OECONF_append_sdx20 = " --with-qmi-cli --with-qxdm"
EXTRA_OECONF_append_sdxpoorwills = " --with-qmi-cli --with-qxdm"

SRC_DIR = "${WORKSPACE}/wlan-proprietary/qcacld-utils/"

S = "${WORKDIR}/wlan-proprietary/qcacld-utils"

