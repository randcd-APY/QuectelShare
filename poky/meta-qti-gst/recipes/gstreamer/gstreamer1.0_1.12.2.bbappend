DEPENDS += "gobject-introspection gobject-introspection-native"
FILESEXTRAPATHS_prepend_apq8098 := "${THISDIR}/qti-patches:"

SRC_URI_append_apq8098 = " \
       file://gstreamer-baseparse-add-suport-zero-copy-for-qtivdec.patch \
"

EXTRA_OECONF = "--libexecdir=${libdir}/${BPN}"
