inherit qcommon qprebuilt autotools-brokensep useradd

DESCRIPTION = "Sensors-see Library"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

DEPENDS = "liblog"
DEPENDS += "libcutils"
DEPENDS += "libutils"
DEPENDS += "system-core"
DEPENDS += "common-headers"
DEPENDS += "glib-2.0"
DEPENDS += "qmi-framework"
DEPENDS += "diag"
DEPENDS += "libhardware"
DEPENDS += "protobuf"
DEPENDS += "protobuf-native"
DEPENDS += "sensors-see-commonsys-intf"
DEPENDS += "qmi"
DEPENDS += "time-genoff"
DEPENDS += "configdb"
DEPENDS += "dsutils"
DEPENDS += "xmllib"

FILESPATH =+ "${WORKSPACE}/vendor/qcom/proprietary/:"
SRC_URI  = "file://sensors-see/"
SRC_URI += "file://sensors-see-daemon.service"

S = "${WORKDIR}/sensors-see"

SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/sensors-see"

CFLAGS += "-I${STAGING_INCDIR}/cutils"
CFLAGS += "-I${STAGING_INCDIR}/utils"
CFLAGS += "-I${STAGING_LIBDIR}/glib-2.0/include"
CFLAGS += "-I${WORKSPACE}/system/core/include"
CPPFLAGS += "-DGOOGLE_PROTOBUF_NO_RTTI"

EXTRA_OECONF += " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF += " --with-glib"
EXTRA_OECONF += " --with-common-includes=${STAGING_INCDIR}"
EXTRA_OECONF += " --enable-sns-le-qcs605"

EXTRA_OEMAKE += " 'CC=${CC}' 'CFLAGS=${CFLAGS}'"


QPERM_SERVICE = "${WORKDIR}/sensors-see-daemon.service"

do_install_append() {
    install -d ${D}/vendor/etc/sensors/config
    install -d ${D}/persist/sensors/registry/config
    for json in ${S}/registry/config/common/*.json; do
        install -m 0644 $json -D ${D}/vendor/etc/sensors/config
        install -m 0644 $json -D ${D}/persist/sensors/registry/config
    done

    install -m 0644 ${S}/ssc/registry/sns_reg_config_le -D ${D}/vendor/etc/sensors/sns_reg_config
    install -m 0644 ${S}/ssc/registry/sns_reg_config_le -D ${D}/persist/sensors/registry/sns_reg_config

    install -d ${D}/persist/sensors/registry/registry

    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_unitdir}/system/
        install -m 0644 ${WORKDIR}/sensors-see-daemon.service -D ${D}${systemd_unitdir}/system/sensors-see-daemon.service
        install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
        # enable the service for multi-user.target
        ln -sf ${systemd_unitdir}/system/sensors-see-daemon.service \
             ${D}${systemd_unitdir}/system/multi-user.target.wants/sensors-see-daemon.service
    fi
}

do_install_append_qcs605() {
    install -d ${D}/vendor/etc/sensors/config
    install -d ${D}/persist/sensors/registry/config
    for json in ${S}/registry/config/qcs605/*.json; do
        install -m 0644 $json -D ${D}/vendor/etc/sensors/config
        install -m 0644 $json -D ${D}/persist/sensors/registry/config
    done
}

do_install_append_qcs40x() {
    install -d ${D}/vendor/etc/sensors/config
    install -d ${D}/persist/sensors/registry/config
    for json in ${S}/registry/config/qcs405/*.json; do
        install -m 0644 $json -D ${D}/vendor/etc/sensors/config
        install -m 0644 $json -D ${D}/persist/sensors/registry/config
    done
}

#Skips check for .so symlinks
INSANE_SKIP_${PN} = "dev-so"

# need to export these variables for python-config to work
FILES_${PN} += "/vendor/etc/sensors/*"
FILES_${PN} += "/persist/sensors/*"
FILES_${PN} += "${systemd_unitdir}/system/"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
