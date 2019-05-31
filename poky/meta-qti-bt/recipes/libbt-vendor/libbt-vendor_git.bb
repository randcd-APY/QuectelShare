inherit autotools-brokensep pkgconfig

DESCRIPTION = "Bluetooth Vendor Library"
HOMEPAGE = "http://codeaurora.org/"
LICENSE = "Apache-2.0"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/\
${LICENSE};md5=89aea4e17d99a7cacdbeed46a0096b10"

DEPENDS = "common system-core hci-qcomm-init glib-2.0"

RDEPENDS_${PN} = "libcutils"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://hardware/qcom/bt/libbt-vendor/"

S = "${WORKDIR}/hardware/qcom/bt/libbt-vendor/"

CFLAGS_append = " -DUSE_ANDROID_LOGGING "
LDFLAGS_append = " -llog "

BASEPRODUCT = "${@d.getVar('PRODUCT', False)}"

EXTRA_OECONF = "--with-common-includes="${WORKSPACE}/vendor/qcom/opensource/bluetooth/hal/include/" \
                --with-lib-path=${STAGING_LIBDIR} \
                --enable-target=${BASEMACHINE} \
                --enable-rome=${BASEPRODUCT} \
                --with-glib \
               "
EXTRA_OECONF_append_quec-smart += "--enable-som=yes"
EXTRA_OECONF_remove_robot-pronto += "--enable-som=yes"
EXTRA_OECONF_append_robot-pronto += "--enable-pronto=yes"

FILES_${PN} += "${sysconfdir}/bluetooth/*"

do_install_append () {
    install -d ${D}${sysconfdir}/bluetooth
    install -m 755 ${S}init.msm.bt.sh ${D}${sysconfdir}/bluetooth/
}
