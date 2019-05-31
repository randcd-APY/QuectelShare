DEPENDS += "python-scons-native"

SCONS_FLAGS ?= "\
    CC="${CC}" \
    CXX="${CXX}" \
    LD="${LD}" \
    AR="${AR}" \
    CFLAGS="${CFLAGS}" \
    CXXFLAGS="${CXXFLAGS}" \
    LINKFLAGS="${LDFLAGS}" \
    STAGING_INCDIR="${STAGING_INCDIR}" \
    OS=linux \
    PLAT_ALIAS="qsap8017" \
    PROVIDED_LIBS=all \
    "

EXTRA_OESCONS ?= ""

do_configure[noexec] = "1"

allplay_scons_do_compile() {
        ${STAGING_BINDIR_NATIVE}/scons ${PARALLEL_MAKE} PREFIX=${prefix} prefix=${prefix} ${SCONS_FLAGS} ${EXTRA_OESCONS} || \
        die "scons build execution failed."
}

allplay_scons_do_install() {
        ${STAGING_BINDIR_NATIVE}/scons PREFIX=${D}${prefix} prefix=${D}${prefix} install ${EXTRA_OESCONS}|| \
        die "scons install execution failed."
}

do_eclipse_scan_gcc() {
        echo  | ${CC} ${CFLAGS} -E -P -v -dD -x c -
}

do_eclipse_scan_gxx() {
        echo | ${CXX} ${CXXFLAGS} -E -P -v -dD -x c++ -
}

addtask do_eclipse_scan_gcc
addtask do_eclipse_scan_gxx

EXPORT_FUNCTIONS do_compile do_install
