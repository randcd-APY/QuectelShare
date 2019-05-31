inherit autotools
PR = "r1"

DESCRIPTION = "IPTH DME tool for FOTA"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

SRC_URI = "file://${WORKSPACE}/Innopath/FOTA/ipth_dme"

S = "${WORKDIR}/ipth_dme"

do_configure(){
    echo "no need to conf ipthdme"
}
do_compile() {
    echo "no need to compile ipthdme"
}

do_install(){
    echo "no need to do_install"
}

INITSCRIPT_NAME = "ipthdme"
INITSCRIPT_PARAMS = "start 42 5 . stop 80 0 1 6 ."
inherit update-rc.d

FILES_${PN} += "/usr/fota"
do_install_append() {
       install  ${WORKDIR}/ipth_dme/dme/usr/fota -d ${D}/usr/fota/
       install -m 0755 ${WORKDIR}/ipth_dme/dme/usr/fota/fumo.dfp -D ${D}/usr/fota/fumo.dfp
       install -m 0755 ${WORKDIR}/ipth_dme/dme/usr/fota/fumo.tre -D ${D}/usr/fota/fumo.tre
       install -m 0755 ${WORKDIR}/ipth_dme/dme/usr/fota/ipth-muc.prop -D ${D}/usr/fota/ipth-muc.prop
       install -m 0755 ${WORKDIR}/ipth_dme/dme/usr/fota/mgmtree.dfp -D ${D}/usr/fota/mgmtree.dfp
       install -m 0755 ${WORKDIR}/ipth_dme/dme/usr/fota/mgmtree.tre -D ${D}/usr/fota/mgmtree.tre
       install -m 0755 ${WORKDIR}/ipth_dme/ipth_dme -D ${D}${bindir}/ipth_dme
       install -m 0755 ${WORKDIR}/ipth_dme/start_ipthdme -D ${D}${sysconfdir}/init.d/ipthdme
}
