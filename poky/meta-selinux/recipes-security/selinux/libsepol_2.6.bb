include selinux_20161014.inc
include ${BPN}.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=a6f89e2100d9b6cdffcea4f398e37343"

SRC_URI[md5sum] = "dc1dfd31aea4c6e4b521f3aa2bddf7de"
SRC_URI[sha256sum] = "d856d6506054f52abeaa3543ea2f2344595a3dc05d0d873ed7f724f7a16b1874"

SRC_URI += "file://0001-src-Makefile-fix-includedir-in-libsepol.pc.patch"
