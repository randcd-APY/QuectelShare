EXTRA_OEMAKE = 'all -C ${S}'
DEPENDS += "glib-2.0"
DEPENDS += "virtual/kernel"
DEPENDS += "libxml-simple-perl-native"
DEPENDS += "libsync"
DEPENDS += "libutils"
DEPENDS += "libcutils"

S      = "${WORKDIR}/adreno200"
SRC_DIR = "${WORKSPACE}/adreno200/"

# Need the kernel headers
EXTRA_OECONF_append += " --with-sanitized-headers=${STAGING_INCDIR}/usr/include"
EXTRA_OECONF_append += " --with-common-includes=${STAGING_INCDIR}"

export ADRENO_OBJ_LIB_PATH = "${WORKDIR}/build"

CFLAGS += " -I${STAGING_KERNEL_BUILDDIR}/usr/include"
CFLAGS += " -I${STAGING_KERNEL_DIR}/include"
CFLAGS += " -DADRENO_BUILD_LINUX_OE"

# Need the glib-2.0 headers
CFLAGS += "-I${STAGING_INCDIR}/glib-2.0"
CFLAGS += "-I${STAGING_LIBDIR}/glib-2.0/include"
CFLAGS += "-D__LE__"

LDFLAGS += "-lglib-2.0 -lpthread"
LDFLAGS += "-lcutils -L${ADRENO_OBJ_LIB_PATH}"

LDLIBS   = "-fPIC ${LDFLAGS}"
TARGET_CC_ARCH += "${LDLIBS}"

# C++ headers added to CXXFLAGS
CXXINC  = "-I${STAGING_INCDIR}/c++"
CXXINC += "-I${STAGING_INCDIR}/c++/${TARGET_SYS}"
CXXINC += "-I${WORKSPACE}/adreno200"

CXXFLAGS ="-fno-default-inline -fno-inline-functions ${CXXINC}"
CXXFLAGS_NOOPT = "-O0 ${CXXINC}"

# Prevent "Makefile.oe" from using its defaults when ARCH is not defined.
export ADRENO_BUILD_LINUX_OE = "1"

# Export LE platform
export LE_BOARD_PLATFORM = "${@base_conditional('BASEMACHINE', 'msm8909', 'msm8909', ' msm8917', d)}"

# Broadcast the Platform Build type, to counter errors with version.h
export TARGET_BUILD_TYPE="DEFAULT"
INHIBIT_PACKAGE_STRIP = "1"

# Enable EGL SUB DRIVER
export TARGET_NEEDS_EGL_SUBDRIVER = "1"

do_compile () {
if [ -d "${WORKSPACE}/adreno200" ]; then
    rm -rf ${WORKSPACE}/adreno200/libEGL_adreno/*.so
    rm -rf ${WORKSPACE}/adreno200/libGLESv2_adreno/*.so
    rm -rf ${WORKSPACE}/adreno200/libGLESv1_CM_adreno/*.so
    rm -rf ${WORKSPACE}/adreno200/llvm/utils/TableGen/*.o
    rm -rf ${WORKSPACE}/adreno200/llvm/tools/clang/utils/TableGen/*.o
    rm -rf ${WORKSPACE}/adreno200/llvm/lib/TableGen/*.o
    oe_runmake ${PARALLEL_MAKE} -f Makefile.linux V=1 CXXFLAGS_NOOPT="${CXXFLAGS_NOOPT}" CXXFLAGS="${CXXFLAGS}" LDLIBS="${LDLIBS}"
else
   echo " No adreno component to compile"
fi
}

do_install () {
if [ -d "$WORKSPACE/adreno200" ]; then
   install -d ${D}/usr/include
   install -d ${D}/usr/include/C2D

   install -m 0644 ${S}/include/private/C2D/c2d2.h ${D}/usr/include/
   install -m 0644 ${S}/c2d30/include/c2dExt.h ${D}/usr/include/

   install -d ${D}/usr/include/EGL

   install -m 0644 ${S}/egl14/include/eglSubDriverApi.h ${D}/usr/include/EGL/

   install -d ${D}/usr/include/GLES
   install -d ${D}/usr/include/GLES2
   install -d ${D}/usr/include/GLES3
   install -d ${D}/usr/include/KHR
   install -d ${D}/usr/include/CL

   install -m 0644 ${S}/include/public/EGL/* ${D}/usr/include/EGL/
   install -m 0644 ${S}/include/public/GLES/* ${D}/usr/include/GLES/
   install -m 0644 ${S}/include/public/GLES2/* ${D}/usr/include/GLES2/
   install -m 0644 ${S}/include/public/GLES3/* ${D}/usr/include/GLES3/
   install -m 0644 ${S}/include/public/KHR/* ${D}/usr/include/KHR/
   install -m 0644 ${S}/include/public/CL/* ${D}/usr/include/CL/

   install -d ${D}/lib/firmware
   for f in $(ls ${S}/*.fw); do
      install -m 0644 $f -D ${D}/lib/firmware
   done

   install -d ${D}${libdir}
   for lib in $(ls ${ADRENO_OBJ_LIB_PATH}/*.so); do
      install -m 0644 ${lib} -D ${D}${libdir}
   done
else
   echo " No adreno component to install"
fi
}

PACKAGES = "${PN}"
FILES_${PN} += "/lib /lib/firmware /lib/firmware/* "
FILES_${PN} += "${libdir}/*"
FILES_${PN} += "/usr/include/*"

INSANE_SKIP_${PN} = "dev-deps"
