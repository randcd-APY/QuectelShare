DESCRIPTION="High quality, one-dimensional sample-rate conversion library"
HOMEPAGE = "http://soxr.sourceforge.net"
SECTION = "audio"

LICENSE = "LGPLv2.1"
LIC_FILES_CHKSUM = "file://COPYING.LGPL;md5=8c2e1ec1540fb3e0beb68361344cba7e"

SRC_URI = "https://sourceforge.net/projects/soxr/files/soxr-${PV}-Source.tar.xz"
SRC_URI[md5sum] = "0866fc4320e26f47152798ac000de1c0"
SRC_URI[sha256sum] = "54e6f434f1c491388cd92f0e3c47f1ade082cc24327bdc43762f7d1eefe0c275"

inherit cmake

#Set source directory
S = "${WORKDIR}/soxr-${PV}-Source"

# Set endianness test result
EXTRA_OECMAKE += "-DHAVE_WORDS_BIGENDIAN_EXITCODE=${@base_conditional('SITEINFO_ENDIANNESS', 'be', '0', '1', d)}"

# Disable tests
EXTRA_OECMAKE += "-DBUILD_TESTS:BOOL=OFF -DBUILD_LSR_TESTS:BOOL=OFF"

# Disable libsamplerate bindings
EXTRA_OECMAKE += "-DWITH_LSR_BINDINGS:BOOL=OFF"

# Make a release build with symbols
EXTRA_OECMAKE += "-DCMAKE_BUILD_TYPE=RelWithDebInfo"

# Remove unwanted directory (/usr/share) from image
do_install_append() {
    rm -rf ${D}${datadir}
}

