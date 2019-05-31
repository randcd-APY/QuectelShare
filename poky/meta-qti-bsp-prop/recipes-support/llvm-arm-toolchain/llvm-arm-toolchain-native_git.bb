inherit native

DESCRIPTION = "QTI clang/llvm compiler"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/\
${LICENSE};md5=89aea4e17d99a7cacdbeed46a0096b10"

PR = "r0"

FILESPATH =+ "${WORKSPACE}/vendor/qcom/proprietary/:"
SRC_URI    = "file://llvm-arm-toolchain-ship"

S = "${WORKDIR}/llvm-arm-toolchain-ship"
INHIBIT_SYSROOT_STRIP = "1"
do_compile[noexec] = "1"

LLVM_VERSION ?= "4.0"

do_install() {
    install -d ${D}/${bindir}/llvm-arm-toolchain/
    install -d ${D}/${bindir}/llvm-arm-toolchain/bin/
    cp -rf ${S}/${LLVM_VERSION}/bin/* ${D}/${bindir}/llvm-arm-toolchain/bin/
    install -d ${D}/${bindir}/llvm-arm-toolchain/lib/
    cp -rf ${S}/${LLVM_VERSION}/lib/* ${D}/${bindir}/llvm-arm-toolchain/lib/
}
