inherit qcommon qlicense autotools update-rc.d qprebuilt

DESCRIPTION = "CSD QMI Server"
DEPENDS = "glib-2.0 qmi-framework acdbloader alsa-intf libhardware audiohal qahw system-media"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/audio-qmi/csd-server/"
S = "${WORKDIR}/audio/mm-audio/audio-qmi/csd-server/"

PR = "r4"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                 --with-glib"

INITSCRIPT_NAME = "csdserver"
INITSCRIPT_PARAMS = "start 45 2 3 4 5 . stop 80 0 1 6 ."
