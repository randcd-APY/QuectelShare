FILESEXTRAPATHS_prepend := "${THISDIR}:"
SRC_URI = "${CAF_GIT}/platform/external/prelink-cross;protocol=git;branch=yocto/cross_prelink \
           file://prelink.conf \
           file://prelink.cron.daily \
           file://prelink.default \
           file://macros.prelink"
PR = "r1"
