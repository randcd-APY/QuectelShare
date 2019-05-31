FILESEXTRAPATHS_prepend := "${THISDIR}/qti-patches:${THISDIR}/gstreamer1.0-plugins-good:"

SRC_URI += "\
	    file://0001-pulse-Expose-the-correct-max-rate-that-we-support.patch \
            "
