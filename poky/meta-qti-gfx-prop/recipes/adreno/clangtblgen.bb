DESCRIPTION = "Clangtblgen"
inherit autotools qcommon perlnative pythonnative cmake qlicense

PR = "r0"

# LLVM need tblgen tool
BBCLASSEXTEND = "native"

DEPENDS = "libxml-simple-perl-native"
DEPENDS_class-target = "clangtblgen-native virtual/kernel glib-2.0"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://adreno200/"
SRC_DIR = "${WORKSPACE}/adreno200/"
S = "${WORKDIR}/adreno200"

OECMAKE_SOURCEPATH_class-native = "${S}/llvm"
OECMAKE_BUILDPATH_class-native = "${WORKDIR}/build"
do_configure_class-native() {
      cmake ${S}/llvm -DCODEPLAY=ON -DLLVM_TARGETS_TO_BUILD:STRING=Oxili -DLLVM_BUILD_32_BITS:BOOL=OFF -DCMAKE_CXX_FLAGS_RELEASE:STRING=-DNDEBUG -DLLVM_INCLUDE_RUNTIME:BOOL=OFF -DLLVM_INCLUDE_TOOLS:BOOL=OFF
}

do_compile_class-native() {
    oe_runmake clang-tblgen
}

INSANE_SKIP_${PN} = "installed-vs-shipped"
do_install_class-native() {
    # include LLVM
    install -d ${D}${bindir}
    install ${WORKDIR}/build/bin/* ${D}${bindir}
}
FILES_${PN} = "${bindir}/*"

INSANE_SKIP_${PN} += "dev-so arch"

FILES_${PN}-dev = ""
