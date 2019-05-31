inherit autotools pkgconfig update-rc.d qprebuilt systemd

DESCRIPTION = "PD mapper"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/\
${LICENSE};md5=92b1d0ceea78229551577d4284669bb8"

PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://ss-services"

SRC_DIR = "${WORKSPACE}/ss-services/"

S = "${WORKDIR}/ss-services"

DEPENDS += "json-c qmi qmi-framework libcutils libutils"

CPPFLAGS += "-I${STAGING_INCDIR}/cutils"
CPPFLAGS += "-I${STAGING_INCDIR}/json-c"
CPPFLAGS += "-I${STAGING_INCDIR}/qmi"
CPPFLAGS += "-I${STAGING_INCDIR}/qmi-framework"

INITSCRIPT_NAME = "start_pdmappersvc"
INITSCRIPT_PARAMS = "start 90 2 3 4 5 . stop 10 0 1 6 ."

do_install_append() {
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
       install -m 0644 ${S}/pd-mapper/pdmapper.service -D ${D}${systemd_unitdir}/system/pdmapper.service
       install -d ${D}/${systemd_unitdir}/system/sysinit.target.wants
       ln -sf ${systemd_unitdir}/system/pdmapper.service ${D}/${systemd_unitdir}/system/sysinit.target.wants/pdmapper.service
    else
       install -m 0755 ${S}/pd-mapper/start_pdmappersvc -D ${D}${sysconfdir}/init.d/start_pdmappersvc
    fi
}

FILES_${PN} += "${systemd_unitdir}/system/*"