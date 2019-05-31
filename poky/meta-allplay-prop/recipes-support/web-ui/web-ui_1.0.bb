SUMMARY = "Web UI"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

SRC_URI = "\
    file://www/ \
    "

RDEPENDS_${PN} += "web-setup"

inherit allarch

SERVER_ROOT="/www/websetup/"

do_install() {
    install -d ${D}${SERVER_ROOT}
    cp -r ${WORKDIR}/www/* ${D}${SERVER_ROOT}/
}

FILES_${PN} += " \
    ${SERVER_ROOT} \
    "
