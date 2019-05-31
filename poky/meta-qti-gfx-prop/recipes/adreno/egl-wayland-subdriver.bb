DESCRIPTION = "EGL Wayland Subdriver"
inherit qlicense
PR = "r0"

DEPENDS = "adreno wayland gbm"
INSANE_SKIP_${PN} = "dev-deps"

FILESPATH =+ "${WORKSPACE}"
SRC_URI = "file://egl-wayland-subdriver/"
SRC_DIR = "${WORKSPACE}/egl-wayland-subdriver/"
S = "${WORKDIR}/egl-wayland-subdriver/"

OECMAKE_SOURCEPATH = "${S}"
OECMAKE_BUILDPATH = "${WORKDIR}/build"

inherit autotools qcommon cmake

EXTRA_OECMAKE = "\
    -DSYSROOT_INCDIR=${STAGING_INCDIR} \
    -DWAYLANDSCANNER_PATH:STRING=${STAGING_BINDIR_NATIVE} \
"

do_install(){
    install -m 0755 -d                                                             ${D}${libdir}/egl-wayland-subdriver
    install -m 0755 ${WORKDIR}/build/waylandegl/libwayland-egl.so.1                ${D}${libdir}/egl-wayland-subdriver
    install -m 0755 ${WORKDIR}/build/eglSubDriverWayland/libeglSubDriverWayland.so ${D}${libdir}/egl-wayland-subdriver
}

FILES_${PN} += "${libdir}/*"
