inherit qcommon qprebuilt qlicense

DESCRIPTION = "Audio Calibration Library"
PR = "r7"

include ${PN}-${BASEMACHINE}.inc

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                 --with-glib"
