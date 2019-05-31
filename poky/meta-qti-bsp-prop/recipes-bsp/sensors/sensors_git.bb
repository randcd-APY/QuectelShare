inherit qcommon qprebuilt useradd

DESCRIPTION = "Sensors Library"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

PR = "r0"

DEPENDS = "liblog libcutils system-core common-headers glib-2.0 qmi-framework diag libhardware"

FILESPATH =+ "${WORKSPACE}/:"
SRC_URI = "file://sensors/"
SRC_URI += "file://sensors-daemon.service"

SRC_DIR = "${WORKSPACE}/sensors/sensors"
S = "${WORKDIR}/sensors/sensors"

CFLAGS += "-I${STAGING_INCDIR}/cutils -I${STAGING_INCDIR}/utils"
LDFLAGS += "-lcutils -lutils"
CFLAGS += "-Dstrlcpy=g_strlcpy"
CFLAGS += "-Dstrlcat=g_strlcat"

EXTRA_OECONF += " --enable-sns-la"
EXTRA_OECONF += " --enable-sns-le-apq8053"
EXTRA_OECONF += " --with-glib"
EXTRA_OECONF += " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OEMAKE += " 'CC=${CC}' 'CFLAGS=${CFLAGS}'"

QPERM_SERVICE = "${WORKDIR}/sensors-daemon.service"

do_install_append() {
    install -d ${D}${sysconfdir}/sensors
    install -m 0644 ${S}/dsps/reg_defaults/sensor_def_qcomdevl.conf -D ${D}${sysconfdir}/sensors/sensor_def_qcomdev.conf
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_unitdir}/system/
        install -m 0644 ${WORKDIR}/sensors-daemon.service -D ${D}${systemd_unitdir}/system/sensors-daemon.service
        install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
        # enable the service for multi-user.target
        ln -sf ${systemd_unitdir}/system/sensors-daemon.service \
             ${D}${systemd_unitdir}/system/multi-user.target.wants/sensors-daemon.service
    fi
}

#Disable the split of debug information into -dbg files
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

#Skips check for .so symlinks
INSANE_SKIP_${PN} = "dev-so"

FILES_${PN} += "${includedir}/*"
FILES_${PN} += "${systemd_unitdir}/system/"
FILES_${PN} += "${libdir}/*.so ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
