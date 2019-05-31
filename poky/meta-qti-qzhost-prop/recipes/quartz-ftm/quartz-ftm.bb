inherit autotools qcommon qprebuilt qlicense

DESCRIPTION = "Qualcomm Technologies ftmdaemon"
DEPENDS = "ftm quartz-host"

PR = "r1"

SRC_DIR = "${WORKSPACE}/wlan-proprietary/ftm/"
QUARTZ_SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/qzhost/"

S = "${WORKDIR}/wlan-proprietary/ftm"
SRC_URI += "file://quartz-ftm.patch"

BASEPRODUCT = "${@d.getVar('PRODUCT', False)}"

EXTRA_OECONF = " \
                --with-glib \
                --enable-quartz=yes \
                --enable-debug=yes \
                --enable-target=${BASEMACHINE} \
                --enable-rome=${BASEPRODUCT} \
                "
do_patch() {
    cd ${S}
    cp ${QUARTZ_SRC_DIR}/quartz-host/exthost/Linux/diag -a .
    cp ${QUARTZ_SRC_DIR}/quartz-host/quartz/serializer/framework/common/qsCommon.h diag/
    cp ${QUARTZ_SRC_DIR}/quartz-host/quartz/serializer/framework/common/qsPack.h diag/
    cp ${QUARTZ_SRC_DIR}/quartz-host/quartz/serializer/framework/host/qsHost.h diag/
    cp ${QUARTZ_SRC_DIR}/quartz-host/quartz/serializer/framework/OSAL/include/qsOSAL.h diag/
    if [ -f ${WORKDIR}/quartz-ftm.patch ]; then
        cd ${S}
        patch -p1 < ${WORKDIR}/quartz-ftm.patch
    fi;
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ftmdaemon_qz -D ${D}${bindir}
}
