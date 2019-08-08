require gstreamer1.0-plugins-good.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=a6f89e2100d9b6cdffcea4f398e37343 \
                    file://common/coverage/coverage-report.pl;beginline=2;endline=17;md5=a4e1830fce078028c8f0974161272607 \
                    file://gst/replaygain/rganalysis.c;beginline=1;endline=23;md5=b60ebefd5b2f5a8e0cab6bfee391a5fe"

SRC_URI = " \
    http://gstreamer.freedesktop.org/src/gst-plugins-good/gst-plugins-good-${PV}.tar.xz \
    file://0001-gstrtpmp4gpay-set-dafault-value-for-MPEG4-without-co.patch \
    file://avoid-including-sys-poll.h-directly.patch \
    file://ensure-valid-sentinel-for-gst_structure_get.patch \
    file://0001-introspection.m4-prefix-pkgconfig-paths-with-PKG_CON.patch \
    file://0001-v4l2-Fix-4K-colorimetry.patch \
"
SRC_URI[md5sum] = "20254217d9805484532e08ff1c3aa296"
SRC_URI[sha256sum] = "5591ee7208ab30289a30658a82b76bf87169c927572d9b794f3a41ed48e1ee96"

S = "${WORKDIR}/gst-plugins-good-${PV}"

# add by Les
DEPENDS += "libcutils"
RDEPENDS_${PN} += "glib-2.0"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${libdir}/*.so.*.*.* ${sysconfdir}/* ${bindir}/* ${libdir}/pkgconfig/* ${base_prefix}/*"
FILES_${PN}      += "${libdir}/gstreamer-1.0/*.so ${libdir}/gstreamer-1.0/*.so.* ${libdir}/gstreamer-1.0/*.so.*.*.*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"

do_install_append() {
    oe_runmake DESTDIR="${D}/" LIBVER="${LV}" install
    install -m 0644 ${WORKSPACE}/prebuilt_HY11/msm8909/gstreamer/gstreamer1.0-plugins-good-quectelmipi/libgstquectelmipi.so -D ${D}${libdir}/gstreamer-1.0/libgstquectelmipi.so
    ln -sf libgstquectelmipi.so ${D}${libdir}/libgstquectelmipi.so.0
}

#INSANE_SKIP_${PN} += "dev-so"
#EXCLUDE_FROM_SHLIBS = "1"

RPROVIDES_${PN}-pulseaudio += "${PN}-pulse"
RPROVIDES_${PN}-soup += "${PN}-souphttpsrc"