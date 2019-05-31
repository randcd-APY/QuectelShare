SUMMARY = "Open Source Fraunhofer FDK AAC library"

LICENSE = "FII-1"
LIC_FILES_CHKSUM = "file://NOTICE;md5=087ae5edf3094fbebf2e44334fa2155c"

SRC_URI = "http://downloads.sourceforge.net/opencore-amr/fdk-aac-${PV}.tar.gz"
SRC_URI += "file://001-prevent-sbr-parse-error-concealment.patch \
            file://002-makefile-std-c++98-style.patch"

SRC_URI[md5sum] = "e274a7d7f6cd92c71ec5c78e4dc9f8b7"
SRC_URI[sha256sum] = "5910fe788677ca13532e3f47b7afaa01d72334d46a2d5e1d1f080f1173ff15ab"

inherit autotools pkgconfig

EXTRA_OECONF = "--enable-static=no --disable-asm"

do_install () {
    oe_runmake install DESTDIR=${D} LIBDIR=${libdir} INCLUDEDIR=${includedir}
}
