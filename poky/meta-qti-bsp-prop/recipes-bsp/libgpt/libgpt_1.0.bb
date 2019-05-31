inherit autotools qlicense

DESCRIPTION = "libgpt library"

PR = "r1"

FILESEXTRAPATHS_prepend := "${THISDIR}:"

SRC_URI = "file://${PN}"

S = "${WORKDIR}/libgpt"
