# Packages installed to the root file system

DISTRO_FEATURES_append = "wayland"

IMAGE_INSTALL += "gstreamer1.0"
IMAGE_INSTALL += "gstreamer1.0-plugins-base"
IMAGE_INSTALL += "gstreamer1.0-plugins-good"
IMAGE_INSTALL += "gstreamer1.0-plugins-bad"
IMAGE_INSTALL += "gstreamer1.0-rtsp-server"
#IMAGE_INSTALL += "gstreamer1.0-plugins-ugly"
#IMAGE_INSTALL += "gstreamer1.0-omx"
#IMAGE_INSTALL += "gstreamer1.0-libav"
