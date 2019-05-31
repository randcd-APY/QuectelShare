inherit autotools-brokensep pkgconfig

DESCRIPTION = "Bluetooth Fluoride Stack"
HOMEPAGE = "http://codeaurora.org/"
LICENSE = "Apache-2.0"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/\
${LICENSE};md5=89aea4e17d99a7cacdbeed46a0096b10"

DEPENDS = "common zlib btvendorhal libbt-vendor system-media"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://system/bt/ \
           file://vendor/qcom/opensource/bluetooth/"

S = "${WORKDIR}/system/bt/"
S_EXT = "${WORKDIR}/vendor/qcom/opensource/bluetooth/system_bt_ext/"


FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}"
FILES_${PN} += "${sysconfdir}/bluetooth/*"
FILES_${PN} += "${userfsdatadir}/misc/bluetooth/*"
FILES_${PN} += "/persist/bluetooth/"
INSANE_SKIP_${PN} = "dev-so"

CFLAGS_append = " -DUSE_ANDROID_LOGGING -DUSE_LIBHW_AOSP"
LDFLAGS_append = " -llog "

BASEPRODUCT = "${@d.getVar('PRODUCT', False)}"

EXTRA_OECONF = " \
                --with-zlib \
                --with-common-includes="${WORKSPACE}/vendor/qcom/opensource/bluetooth/hal/include/" \
                --with-lib-path=${STAGING_LIBDIR} \
                --enable-target=${BASEMACHINE} \
                --enable-rome=${BASEPRODUCT} \
               "

EXTRA_OECONF_append_quec-smart += "--enable-som=yes"
EXTRA_OECONF_remove_robot-pronto += "--enable-som=yes"
EXTRA_OECONF_append_robot-pronto += "--enable-pronto=yes"

do_install_append() {

	install -d ${D}${sysconfdir}/bluetooth/
	install -d ${D}${userfsdatadir}/misc/bluetooth/
	install -d ${D}/persist/bluetooth

	cd  ${D}/${libdir}/ && ln -s libbluetoothdefault.so.0 bluetooth.default.so
	cd  ${D}/${libdir}/ && ln -s libaudioa2dpdefault.so.0 audio.a2dp.default.so

	if [ -f ${S}conf/auto_pair_devlist.conf ]; then
	   install -m 0660 ${S}conf/auto_pair_devlist.conf ${D}${sysconfdir}/bluetooth/
	fi

#	if [ -f ${S_EXT}conf/interop_database.conf ]; then
#	   install -m 0660 ${S_EXT}conf/interop_database.conf ${D}${userfsdatadir}/misc/bluetooth/
#	fi

	if [ -f ${S}conf/bt_did.conf ]; then
	   install -m 0660 ${S}conf/bt_did.conf ${D}${sysconfdir}/bluetooth/
	fi

	if [ -f ${S}conf/bt_stack.conf ]; then
	   install -m 0660 ${S}conf/bt_stack.conf ${D}${sysconfdir}/bluetooth/
	fi

	if [ -f ${S}conf/iot_devlist.conf ]; then
	   install -m 0660 ${S}conf/iot_devlist.conf ${D}${sysconfdir}/bluetooth/
	   install -m 0660 ${S}conf/iot_devlist.conf ${D}${userfsdatadir}/misc/bluetooth/
	fi
}
INHIBIT_PACKAGE_DEBUG_SPLIT="1"
