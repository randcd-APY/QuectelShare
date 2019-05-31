inherit autotools qlicense

DESCRIPTION = "abctl library and utility."

PR = "r1"

DEPENDS += "libgpt"

FILESEXTRAPATHS_prepend := "${THISDIR}:"

SRC_URI = "file://${PN}"

S = "${WORKDIR}/abctl"
