include selinux_20161014.inc
include ${BPN}.inc

LIC_FILES_CHKSUM = "file://LICENSE;md5=84b4d2c6ef954a2d4081e775a270d0d0"

SRC_URI[md5sum] = "0e066ba6d6e590ba4b53eed64905d901"
SRC_URI[sha256sum] = "4ea2dde50665c202253ba5caac7738370ea0337c47b251ba981c60d24e1a118a"

SRC_URI += "\
        file://libselinux-drop-Wno-unused-but-set-variable.patch \
        file://libselinux-make-O_CLOEXEC-optional.patch \
        file://libselinux-make-SOCK_CLOEXEC-optional.patch \
        file://libselinux-define-FD_CLOEXEC-as-necessary.patch \
        file://0001-src-Makefile-fix-includedir-in-libselinux.pc.patch \
        "
