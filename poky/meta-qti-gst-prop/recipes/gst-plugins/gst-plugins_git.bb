deltask do_package_qa

DEFAULT_PREFERENCE = "-1"

include gst-plugins.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=c5e7ec9b8d9955820c3d2536d9e434cd \
                 "
DEPENDS  += "gstreamer1.0 gstreamer1.0-plugins-base orc audiohal qsthw-api"
DEPENDS_append_apq8017 = " media"
DEPENDS_append_msm8909 = " media"

FILESPATH =+ "${WORKSPACE}:"

EXTRA_OECONF_append_apq8017 = " --enable-omx"
EXTRA_OECONF_append_msm8909 = " --enable-omx"


SRC_URI = "file://vendor/qcom/proprietary/gst-plugins/"
S = "${WORKDIR}/vendor/qcom/proprietary/gst-plugins/"
SRCREV = "${AUTOREV}"
FILES_${PN} += "/etc/*"

do_configure_prepend() {
    cd ${S}
    chmod +x ./autogen.sh
    ./autogen.sh --noconfigure
    cd ${B}
}

