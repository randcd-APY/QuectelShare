inherit autotools qlicense

DESCRIPTION = "mm-video Proprietary codecs for MSM chipsets"

PR = "r7"

SRC_URI = "file://${WORKSPACE}/mm-video-prop-ship"
S = "${WORKDIR}/mm-video-prop-ship"

DEPENDS = "mm-core-oss mm-video-oss system-core"

PACKAGE_ARCH = "${MACHINE_ARCH}"

EXTRA_OECONF_append = " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF_append = " --with-common-includes=${STAGING_INCDIR}"
EXTRA_OECONF_append = " --with-additional-include-directives="-I${WORKSPACE}/base/include -I${WORKSPACE}/system/core/include -I${WORKSPACE}/hardware/libhardware/include""
EXTRA_OECONF_append = " --with-video-prop-ship-srcpath=${WORKSPACE}/mm-video-prop-ship"
EXTRA_OECONF_append = " --with-video-oss-srcpath=${WORKSPACE}/mm-video-oss"
EXTRA_OECONF_append = " --enable-target-${BASEMACHINE}=yes"
EXTRA_OECONF_append = " --enable-jelly-bean=no"

CPPFLAGS += "-I${STAGING_INCDIR}/glib-2.0"
CPPFLAGS += "-I${STAGING_LIBDIR}/glib-2.0/include"
CPPFLAGS += "-I${STAGING_INCDIR}/mm-core"
CXXFLAGS += "-I${STAGING_INCDIR}/c++"
CXXFLAGS += "-I${STAGING_INCDIR}/c++/${TARGET_SYS}"

LDFLAGS += "-lglib-2.0"

PACKAGES = "${PN}"
#Disable the split of debug information into -dbg files
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

FILES_${PN} = "\
    /usr/lib/* \
    /usr/bin/* \
    /usr/share/*"

#Skips check for .so symlinks
INSANE_SKIP_${PN} = "dev-so"

do_install() {
    oe_runmake DESTDIR="${D}/" install
}
do_configure[depends] += "virtual/kernel:do_shared_workdir"
