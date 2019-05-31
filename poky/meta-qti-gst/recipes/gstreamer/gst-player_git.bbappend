FILESEXTRAPATHS_prepend := "${THISDIR}/gst-player:"

SRC_URI += "file://filechooser.patch;apply=0 \
            file://Fix-pause-play.patch;apply=0 \
            file://Add-error-signal-emission-for-missing-plugins.patch;apply=0 \
            file://0001-gtk-play-Disable-visualizations.patch"

DISTRO_FEATURES_append = " wayland"
