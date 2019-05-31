DESCRIPTION = "An advanced alternative, asynchronous resolver."
SECTION = "console/network"
LICENSE="GPL"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD;md5=3775480a712fc46a69647678acb234cb"

inherit autotools-brokensep

SRC_URI = "http://www.chiark.greenend.org.uk/~ian/adns/ftp/adns-${PV}.tar.gz"

do_configure() {
        ./configure --host=arm-oe-linux-gnueabi --includedir=${STAGING_INCDIR} \
        --prefix=${D}${prefix}
}

SRC_URI[md5sum] = "8b6274d314af0de1a00cf3ef49f6dd56"
SRC_URI[sha256sum] = "5b1026f18b8274be869245ed63427bf8ddac0739c67be12c4a769ac948824eeb"
