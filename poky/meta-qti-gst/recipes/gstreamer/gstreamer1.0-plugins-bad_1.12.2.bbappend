DEPENDS += "gobject-introspection gobject-introspection-native"

CPPFLAGS += " ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', '-D__GBM__', '', d)} "

DEPENDS_append_apq8098 = " wayland wayland-native libdrm weston"

FILESEXTRAPATHS_prepend_apq8098 := "${THISDIR}/qti-patches:"
SRC_URI_append_apq8098 = " \
       file://waylandsink-Add-support-for-gbm-buffer-backend.patch \
       file://gst-bad-plugins-h264-h265-zero-copy-support-for-qtivdec.patch \
       file://waylandsink-Add-fullscreen-support.patch \
"

python do_after_patch_apq8098 () {
    import os

    cmd = "install -d ${S}/gst-libs/gst/ionbuf/ && (echo -n "" > ${S}/gst-libs/gst/ionbuf/Makefile.am)"
    os.system(cmd)
}

addtask after_patch_apq8098 after do_patch_apq8098
