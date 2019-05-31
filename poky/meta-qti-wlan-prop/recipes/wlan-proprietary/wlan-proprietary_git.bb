inherit autotools

DESCRIPTION = "Qualcomm Atheros WLAN"
HOMEPAGE = "http://support.cdmatech.com"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

FILES_${PN} += "${base_libdir}/firmware/ath6k/AR6003/hw2.1.1/*"

PR = "r2"

DEPENDS = "common diag glib-2.0"

SRC_URI = "file://${WORKSPACE}/wlan-proprietary"

S = "${WORKDIR}/wlan-proprietary/host"

ATH_BUILD_TYPE = "${BASEMACHINE}"
ATH_BUS_TYPE   = "SDIO"

EXTRA_OEMAKE = "\
    ATH_LINUXPATH=${STAGING_DIR_HOST}${base_libdir}/modules/${KERNEL_VERSION}/build \
    ATH_CROSS_COMPILE_TYPE=${STAGING_BINDIR_TOOLCHAIN}/${HOST_PREFIX} \
    ATH_BUILD_TYPE=${ATH_BUILD_TYPE} \
    ATH_BUS_TYPE=${ATH_BUS_TYPE} \
    ATH_SRC_BASE=${WORKSPACE}/wlan/host \
    ATH_OS_TYPE=linux_3_0 \
    ATH_ARCH_CPU_TYPE=${TARGET_ARCH} \
    ATH_BUS_SUBTYPE=linux_sdio \
    ATH_BUILD_FTM=yes \
    LDFLAGS='${LDFLAGS}' \
    SYSROOT='--sysroot=${STAGING_DIR_TARGET}' \
    "

EXTRA_OEMAKE_append_mdm9625 = "ATH_TOOLS_POSTFIX=_wext"

ATH_FTM_CONFIG = "${S}/tools/ftmdaemon/.config"
ATH_WMI_CONFIG = "${S}/tools/wmiconfig/.config"

ATH_SRC_FIRMWARE_DIR = "${S}/../target/AR6003/hw2.1.1/bin"
ATH_DEST_FIRMWARE_DIR = "${D}${base_libdir}/firmware/ath6k/AR6003/hw2.1.1"

CFLAGS_append    = " -I${STAGING_INCDIR}"
LDFLAGS_append   = " -L${STAGING_LIBDIR} -lglib-2.0 -lgthread-2.0 -ldiag"

do_configure() {
    echo "CFLAGS  += -I${STAGING_INCDIR}"        > ${ATH_FTM_CONFIG}
    echo "CFLAGS  += -I${STAGING_INCDIR}/diag"  >> ${ATH_FTM_CONFIG}
    echo "CFLAGS  += -I${WORKSPACE}/common/inc" >> ${ATH_FTM_CONFIG}
    echo "LDFLAGS += -L${STAGING_LIBDIR}"       >> ${ATH_FTM_CONFIG}
    echo "LDFLAGS +=   ${LDFLAGS}"              >> ${ATH_FTM_CONFIG}

    echo "KERNEL_SRC = ${STAGING_KERNEL_DIR}/usr"           > ${ATH_WMI_CONFIG}
    echo "CFLAGS  += -I${STAGING_INCDIR}"                  >> ${ATH_WMI_CONFIG}
    echo "CFLAGS  += -I${STAGING_INCDIR}/glib-2.0"         >> ${ATH_WMI_CONFIG}
    echo "CFLAGS  += -I${STAGING_LIBDIR}/glib-2.0/include" >> ${ATH_WMI_CONFIG}
    echo "LDFLAGS +=   ${LDFLAGS}"                         >> ${ATH_WMI_CONFIG}
}

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}${base_sbindir}
    install -d ${ATH_DEST_FIRMWARE_DIR}

    install -m 0644 ${ATH_SRC_FIRMWARE_DIR}/* ${ATH_DEST_FIRMWARE_DIR}

    oe_runmake install ATH_BUILD_OUTPUT_OVERRIDE=${D}${base_sbindir}
}
