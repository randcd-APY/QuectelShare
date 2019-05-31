SUMMARY = "A free C++ class library of cryptographic schemes"
BUGTRACKER = "http://sourceforge.net/apps/trac/cryptopp/"
SECTION = "libs"

LICENSE = "BSL-1.0"
LIC_FILES_CHKSUM = "file://License.txt;md5=deb6d182b0f7f8a866c42941b9f014c4"

BBCLASSEXTEND = "native nativesdk"

PR = "r1"

PVSHORT = "${@'${PV}'.replace('.','')}"
SRC_URI = " \
    https://sourceforge.net/projects/cryptopp/files/cryptopp/5.6.4/cryptopp564.zip;subdir=libcryptopp-${PV} \
    file://0001-Fix-cross-compilation.patch \
"
SRC_URI[md5sum] = "4ee7e5cdd4a45a14756c169eaf2a77fc"
SRC_URI[sha256sum] = "be430377b05c15971d5ccb6e44b4d95470f561024ed6d701fe3da3a188c84ad7"

inherit autotools-brokensep pkgconfig

PACKAGES += "${PN}-test "

EXTRA_OECONF = "--libdir=${base_libdir}"
TARGET_CC_ARCH += "${LDFLAGS}"
export PREFIX="${prefix}"

#we want tegrarcm binary to run on a 32-bit architecture, on x86_64 this requires the 32-bit compatibility libs
EXTRA_OEMAKE_class-native = "CC='${CC} -m32' CXX='${CXX} -m32'"

do_compile() {
    sed -i -e 's/^CXXFLAGS/#CXXFLAGS/' GNUmakefile
    export CXXFLAGS="${CXXFLAGS} -DNDEBUG -fPIC"
    oe_runmake all libcryptopp.so
}

# do not provide the shared object file, so we force to link statically for host tools
do_compile_class-native() {
    sed -i -e 's/^CXXFLAGS/#CXXFLAGS/' GNUmakefile
    export CXXFLAGS="${CXXFLAGS} -DNDEBUG -fPIC"
    oe_runmake all
}

FILES_${PN}-test = " \
    ${bindir} \
    ${datadir}/cryptopp \
"

PACKAGES = "${PN}"
FILES_${PN} += "/usr/lib/*"

do_package_qa() {
}
