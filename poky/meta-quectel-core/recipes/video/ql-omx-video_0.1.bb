inherit autotools pkgconfig sdllvm

DESCRIPTION = "ql-omx-video"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-core/ql-omx-video"

S = "${WORKDIR}/quectel-core/ql-omx-video"
SRC_DIR = "${WORKSPACE}/quectel-core/ql-omx-video"

DEPENDS += "libcutils"
RDEPENDS_${PN} += "glib-2.0"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${libdir}/*.so.*.*.* ${sysconfdir}/* ${bindir}/* ${libdir}/pkgconfig/* ${base_prefix}/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
do_install_append() {
 	oe_runmake DESTDIR="${D}/" LIBVER="${LV}" install
	install -m 0644 ${WORKSPACE}/quectel-core/ql-omx-video/lib/libqcamera-omx.so -D ${D}${libdir}/libqcamera-omx.so
	ln -sf libqcamera-omx.so ${D}${libdir}/libqcamera-omx.so.0
}

INSANE_SKIP_${PN} += "dev-so"
EXCLUDE_FROM_SHLIBS = "1"
