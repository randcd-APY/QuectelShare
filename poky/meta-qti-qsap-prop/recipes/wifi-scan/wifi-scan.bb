SUMMARY = "Wifi scan list generator"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

FILESPATH_prepend = "${WORKSPACE}/vendor/qcom/proprietary/web-setup:"
SRC_URI = "\
           file://wifi-scan \
           "
S = "${WORKDIR}/wifi-scan"

DEPENDS = "boost libnl"

# Command line only tool, there is no point in allowing multilib
MULTILIBS = ""

inherit pkgconfig

do_compile() {
    ${CXX} ${CXXFLAGS} ${LDFLAGS} -Wl,--no-as-needed $(pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0) -o wifi_scan wifi_scan.cpp
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/wifi_scan ${D}${bindir}
}

