DESCRIPTION = "Qualcomm Atheros WLAN Host Driver Module"
SECTION = "kernel/modules"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

PR = "r0"

DEPENDS = "virtual/kernel"

SRC_URI = "file://${WORKSPACE}/vendor/qcom/proprietary/wlan/prima"

S = "${WORKDIR}/prima"

inherit module qperf

do_compile () {
    unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS CC CPP LD
    oe_runmake 'MODPATH="${base_libdir}/modules/wlan/prima"' \
        'KERNEL_SOURCE="${STAGING_KERNEL_DIR}"' \
        'KDIR="${STAGING_KERNEL_DIR}"' \
        'CC="${KERNEL_CC}"' \
        'LD="${KERNEL_LD}"' \
        'WLAN_DIR="${S}"'
}

do_install () {
    install -d ${D}${base_libdir}/modules/wlan/prima
    install -m 0644 ${S}/wlan_prima.ko ${D}${base_libdir}/modules/wlan/prima
}
