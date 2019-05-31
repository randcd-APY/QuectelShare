inherit perlnative pythonnative cmake

# LLVM need tblgen tool
BBCLASSEXTEND = "native"

RDEPENDS_${PN}        = "${@base_conditional('SUPPORTS_WAYLAND_FRAMEWORK', '1', 'wayland', ' ', d)}"
RDEPENDS_${PN}-dev    = "${@base_conditional('SUPPORTS_WAYLAND_FRAMEWORK', '1', 'gbm', ' ', d)}"

DEPENDS               = "libxml-simple-perl-native ${@base_conditional('BASEMACHINE', 'apq8098', 'wayland', ' ', d)}"
DEPENDS              += "libxml-simple-perl-native ${@base_conditional('BASEMACHINE', 'qcs605', 'wayland', ' ', d)}"
DEPENDS              += "libxml-simple-perl-native ${@base_conditional('BASEMACHINE', 'qcs40x', 'wayland', ' ', d)}"

DEPENDS_class-target  = "${@base_conditional('SUPPORTS_ANDROID_FRAMEWORK', '1', 'binder libsync libutils libui display-hal', ' ', d)}"
DEPENDS_class-target  += "clangtblgen-native adreno-native virtual/kernel glib-2.0 liblog libcutils"
DEPENDS_class-target  += "${@base_conditional('SUPPORTS_WAYLAND_FRAMEWORK', '1', 'wayland', ' ', d)}"
DEPENDS_class-target  += "${@base_conditional('SUPPORTS_WAYLAND_FRAMEWORK', '1', 'gbm', ' ', d)}"
DEPENDS_class-target  += "${@base_conditional('SUPPORTS_WAYLAND_FRAMEWORK', '1', 'drm', ' ', d)}"
DEPENDS_class-target  += "${@base_conditional('SUPPORTS_KGSL', '1', 'libsync libutils', ' ', d)}"
DEPENDS_class-target  += "${@base_conditional('BASEMACHINE', 'qcs40x', 'libion', ' ', d)}"
DEPENDS_class-target  += "${@base_conditional('BASEMACHINE', 'qcs40x', 'libsync', ' ', d)}"

#DEPENDS = "libxml-simple-perl-native"
#RDEPENDS_${PN} = "wayland"
#RDEPENDS_${PN}-dev = "gbm"
#DEPENDS_class-target = "clangtblgen-native adreno-native virtual/kernel glib-2.0"

PROVIDES        += "virtual/libgles1 virtual/libgles2 virtual/egl"
RPROVIDES_${PN} += "virtual/libgles1 virtual/libgles2 virtual/egl"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://adreno200/"
SRC_DIR = "${WORKSPACE}/adreno200/"
S = "${WORKDIR}/adreno200"

OECMAKE_SOURCEPATH_class-native = "${S}/khronos/shared/llvm"
OECMAKE_BUILDPATH_class-native = "${WORKDIR}/build"

install_c2d_libs() {
    install -m 0644 ${WORKDIR}/build/C2D/libC2D2.so                    ${D}${libdir}/
    if [ "${@bb.utils.contains('VIPERTOOTH_BUILD', '0', 'TRUE', 'FALSE', d)}" == "TRUE" ]; then
        install -m 0644 ${WORKDIR}/build/c2d30-a5xx/libc2d30-a5xx.so       ${D}${libdir}/
    else
        install -m 0644 ${WORKDIR}/build/c2d30-a3xx/libc2d30-a3xx.so       ${D}${libdir}/
    fi
    install -m 0644 ${WORKDIR}/build/c2d30_bltlib/libc2d30_bltlib.so   ${D}${libdir}/
}

do_configure_class-native() {
      cmake ${S}/khronos/shared/llvm -DCODEPLAY=ON -DLLVM_TARGETS_TO_BUILD:STRING=Oxili -DLLVM_BUILD_32_BITS:BOOL=OFF -DCMAKE_CXX_FLAGS_RELEASE:STRING=-DNDEBUG -DLLVM_INCLUDE_RUNTIME:BOOL=OFF -DLLVM_INCLUDE_TOOLS:BOOL=OFF
}

do_compile_class-native() {
    oe_runmake llvm-tblgen
}

do_install_class-native() {
    # include LLVM
    install -d ${D}${bindir}
    install ${WORKDIR}/build/bin/llvm-tblgen ${D}${bindir}
}

OECMAKE_SOURCEPATH = "${S}/build/cmake/"
OECMAKE_BUILDPATH = "${WORKDIR}/build"
EXTRA_OECONF_append = "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF_append = "--with-glib"

EXTRA_OECMAKE = "\
      -DCODEPLAY=ON \
      -DLLVM_TARGETS_TO_BUILD:STRING=Oxili \
      -DLLVM_INCLUDE_RUNTIME:BOOL=OFF \
      -DLLVM_INCLUDE_TOOLS:BOOL=OFF \
      -DCONFIG:STRING=Release \
      -DPLATFORM:STRING=linux \
      -DCPU:STRING=64 \
      -DADRENO_DRIVER:STRING=${S} \
      -DGLIB2_PATH:STRING=${STAGING_INCDIR}/glib-2.0 \
      -DGLIB2_INTERNAL_PATH:STRING=${STAGING_LIBDIR}/glib-2.0/include \
      -DCMAKE_CROSSCOMPILING=True \
      -DLLVM_TABLEGEN=${STAGING_BINDIR_NATIVE}/llvm-tblgen \
      -DWAYLANDSCANNER_PATH=${STAGING_BINDIR_NATIVE} \
      -DSYSROOT_LIBDIR=${STAGING_LIBDIR} \
      -DSYSROOT_INCDIR=${STAGING_INCDIR} \
      -DKERNEL_INCDIR=${STAGING_KERNEL_BUILDDIR} \
      -DWAYLAND:BOOL=${@base_conditional('SUPPORTS_WAYLAND_FRAMEWORK', '1', 'ON', 'OFF', d)} \
      -DLE_USES_ANDROID:BOOL=${@base_conditional('SUPPORTS_ANDROID_FRAMEWORK', '1', 'ON', 'OFF', d)} \
      -DFBDEV:BOOL=${@base_conditional('SUPPORTS_FBDEV', '1', 'ON', 'OFF', d)} \
      -DMSM_DRM:BOOL=${@base_conditional('SUPPORTS_DRM', '1', 'ON', 'OFF', d)} \
      -DMSM_KGSL:BOOL=${@base_conditional('SUPPORTS_KGSL', '1', 'ON', 'OFF', d)} \
      -DA3X_ESX:BOOL=${@base_conditional('BASEMACHINE', 'qcs40x', 'ON', 'OFF', d)} \
      -DSCRIPT_PATH:STRING=${COREBASE}/../android_compat/common/scripts \
      -DENABLE_LIBION:BOOL=${@base_conditional('BASEMACHINE', 'qcs40x', 'ON', 'OFF', d)} \
      -DENABLE_C2D:BOOL=${@base_conditional('SUPPORTS_C2D', '1', 'ON', 'OFF', d)} \
      -DENABLE_DYNAMIC_SYMBOL_LOADING:BOOL=${@base_conditional('SUPPORTS_DYNAMIC_SYMBOL_LOADING', '1', 'ON', 'OFF', d)} \
      -DANDROID_LOGGING=ON \
      -DGPU:STRING=${SUPPORTS_GPU} \
"

# Debug
EXTRA_OEMAKE += "VERBOSE=1"
EXTRA_OEMAKE += "USE_ESX=1"
EXTRA_OECMAKE_BUILD += "USE_ESX=1"

do_configure_prepend() {
    perl ${S}/opengl/esx/build/tools/processconfigfile.pl \
                                        ${S}/opengl/esx/core/esxsettings.xml \
                                        ${S}/opengl/esx/hwl/a3x/a3xsettings.xml \
                                        ${S}/opengl/esx/hwl/a5x/a5xsettings.xml \
                                        ${S}/opengl/esx/hwl/a6x/a6xsettings.xml \
                                        ${S}/opengl/esx/build/cmake/esx_config.txt
}

INSANE_SKIP_${PN} = "installed-vs-shipped dev-deps"

do_install_append_class-target(){

   install -d ${D}/data/misc/gpu
   cp ${S}/opengl/esx/build/cmake/esx_config.txt ${D}/data/misc/gpu

   install -d ${D}/lib/firmware

   install -d ${D}${includedir}
   install -d ${D}${includedir}/adreno
   install -d                                           ${D}${libdir}/

   cp -rf ${S}/opengl/esx/shared/include/public/*       ${D}${includedir}
   cp -rf ${S}/opengl/esx/shared/include/public/*       ${D}${includedir}/adreno
   cp -rf ${S}/include/public/CL                        ${D}${includedir}
   cp -rf ${S}/include/public/CL                        ${D}${includedir}/adreno

   install -m 0644 ${WORKDIR}/build/adreno_utils/libadreno_utils.so ${D}${libdir}/
   install -m 0644 ${WORKDIR}/build/gsl/libgsl.so                   ${D}${libdir}/
   install -m 0644 ${WORKDIR}/build/llvm-glnext/libllvm-glnext.so    ${D}${libdir}/
   install -m 0644 ${WORKDIR}/build/q3dtools/libq3dtools_esx.so      ${D}${libdir}/
   install -m 0644 ${WORKDIR}/build/qxprofiler/libq3dtools_adreno.so ${D}${libdir}/

   if [ "${@bb.utils.contains('VIPERTOOTH_BUILD', '0', 'TRUE', 'FALSE', d)}" == "TRUE" ]; then
       install -m 0644 ${WORKDIR}/build/llvm-qcom/libllvm-qcom.so        ${D}${libdir}/
       install -m 0644 ${WORKDIR}/build/cb/libCB.so                      ${D}${libdir}/
       install -m 0644 ${WORKDIR}/build/cb/libOpenCL.so                  ${D}${libdir}/
       install -m 0644 ${WORKDIR}/build/qtap/libQTapGLES.so              ${D}${libdir}/
   fi

   install -m 0644 ${WORKDIR}/build/eglentry/libEGL_adreno.so            ${D}${libdir}/
   install -m 0644 ${WORKDIR}/build/gles11v1entry/libGLESv1_CM_adreno.so ${D}${libdir}/
   install -m 0644 ${WORKDIR}/build/glesentry/libGLESv2_adreno.so        ${D}${libdir}/

   #install -m 0644 ${WORKDIR}/build/eglsubdrivers/wayland/libeglSubDriverWayland.so    ${D}${libdir}/
   #install -m 0644 ${WORKDIR}/build/eglsubdrivers/wayland/waylandegl/libwaylandegl.so  ${D}${libdir}/

   # EGL - Libs
   ln -sf libEGL_adreno.so                               ${D}${libdir}/libEGL.so.1.0.0
   ln -sf libEGL.so.1.0.0                                ${D}${libdir}/libEGL.so.1.0
   ln -sf libEGL.so.1.0.0                                ${D}${libdir}/libEGL.so.1
   ln -sf libEGL.so.1.0.0                                ${D}${libdir}/libEGL.so

   # GLES - Libs
   ln -sf libGLESv1_CM_adreno.so                      ${D}${libdir}/libGLESv1_CM.so.1.0.0
   ln -sf libGLESv1_CM.so.1.0.0                       ${D}${libdir}/libGLESv1_CM.so.1.0
   ln -sf libGLESv1_CM.so.1.0.0                       ${D}${libdir}/libGLESv1_CM.so.1
   ln -sf libGLESv1_CM.so.1.0.0                       ${D}${libdir}/libGLESv1_CM.so

   # GLES2 - Libs
   ln -sf libGLESv2_adreno.so                      ${D}${libdir}/libGLESv2.so.2.0.0
   ln -sf libGLESv2.so.2.0.0                       ${D}${libdir}/libGLESv2.so.2.0
   ln -sf libGLESv2.so.2.0.0                       ${D}${libdir}/libGLESv2.so.2
   ln -sf libGLESv2.so.2.0.0                       ${D}${libdir}/libGLESv2.so

   # fbdev-egl - Libs
   # ln -sf libeglSubdriverFbdev.so                ${D}${libdir}/libeglFbdev.so.1.0.0
   # ln -sf libeglFbdev.so.1.0.0                   ${D}${libdir}/libeglFbdev.so.1.0
   # ln -sf libeglFbdev.so.1.0.0                   ${D}${libdir}/libeglFbdev.so.1
   # ln -sf libeglFbdev.so.1.0.0                   ${D}${libdir}/libeglFbdev.so

   # wayland-egl - Libs
   #ln -sf libwaylandegl.so                       ${D}${libdir}/libwayland-egl.so.1.0.0
   #ln -sf libwayland-egl.so.1.0.0                ${D}${libdir}/libwayland-egl.so.1.0
   #ln -sf libwayland-egl.so.1.0.0                ${D}${libdir}/libwayland-egl.so.1
   #ln -sf libwayland-egl.so.1.0.0                ${D}${libdir}/libwayland-egl.so

   if [ "${@bb.utils.contains('SUPPORTS_FBDEV', '1', 'TRUE', 'FALSE', d)}" == "TRUE" ]; then
       #fbdev-egl - Libs
       ln -sf libeglSubdriverFbdev.so                ${D}${libdir}/libeglFbdev.so.1.0.0
       ln -sf libeglFbdev.so.1.0.0                   ${D}${libdir}/libeglFbdev.so.1.0
       ln -sf libeglFbdev.so.1.0.0                   ${D}${libdir}/libeglFbdev.so.1
       ln -sf libeglFbdev.so.1.0.0                   ${D}${libdir}/libeglFbdev.so
   fi

   if [ "${@bb.utils.contains('SUPPORTS_WAYLAND_FRAMEWORK', '1', 'TRUE', 'FALSE', d)}" == "TRUE" ]; then
       # wayland-egl - Libs
       install -m 0644 ${WORKDIR}/build/eglsubdrivers/wayland/libeglSubDriverWayland.so       ${D}${libdir}/
       install -m 0644 ${WORKDIR}/build/eglsubdrivers/wayland/waylandegl/libwayland-egl.so.1  ${D}${libdir}/
       ln -sf libwayland-egl.so.1                       ${D}${libdir}/libwayland-egl.so
       ln -sf libwayland-egl.so.1                       ${D}${libdir}/libwaylandegl.so
   fi

   if [ "${@bb.utils.contains('SUPPORTS_ANDROID_FRAMEWORK', '1', 'TRUE', 'FALSE', d)}" == "TRUE" ]; then
       #android-le-egl-subdriver - Libs
       install -m 0644 ${WORKDIR}/build/eglsubdrivers/le/libeglSubDriverLE.so    ${D}${libdir}/eglSubDriverLE.so
   fi
}

do_install_append_qcs40x(){

   cp -rf ${S}/include/private/C2D/c2d2.h       ${D}${includedir}
   cp -rf ${S}/include/private/C2D/c2d2.h       ${D}${includedir}/adreno
   cp -rf ${S}/c2d30/include/c2dExt.h           ${D}${includedir}
   cp -rf ${S}/c2d30/include/c2dExt.h           ${D}${includedir}/adreno

   #Install C2D libs
   install_c2d_libs

   #Deciding firmware installation directory
   if [ "${MLPREFIX}" == "lib32-" ] || [ "${MLPREFIX}" == "" -a "${TUNE_ARCH}" == "arm" ]; then
       export FIRMWARE_INSTALL_DIR=${D}/lib/firmware
   else
       install -d ${D}/lib64/firmware
       export  FIRMWARE_INSTALL_DIR=${D}/lib64/firmware
   fi
   # A3X-only firmware
   install -m 0644 ${WORKDIR}/build/firmware/a3x/a300_pfp.fw  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a3x/a300_pm4.fw  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a3x/a330_pfp.fw  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a3x/a330_pm4.fw  ${FIRMWARE_INSTALL_DIR}
}

do_install_append_apq8053(){

   cp -rf ${S}/include/private/C2D/c2d2.h       ${D}${includedir}
   cp -rf ${S}/include/private/C2D/c2d2.h       ${D}${includedir}/adreno
   cp -rf ${S}/c2d30/include/c2dExt.h           ${D}${includedir}
   cp -rf ${S}/c2d30/include/c2dExt.h           ${D}${includedir}/adreno

   #Install C2D libs
   install_c2d_libs

   #Deciding firmware installation directory
   if [ "${MLPREFIX}" == "lib32-" ] || [ "${MLPREFIX}" == "" -a "${TUNE_ARCH}" == "arm" ]; then
       export FIRMWARE_INSTALL_DIR=${D}/lib/firmware
   else
       install -d ${D}/lib64/firmware
       export  FIRMWARE_INSTALL_DIR=${D}/lib64/firmware
   fi

   # currently, all A5X chips use the A530 CP firmware
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530_pfp.fw   ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530_pm4.fw   ${FIRMWARE_INSTALL_DIR}

   # A506-only firmware
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a506_zap.elf  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a506_zap.mdt  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a506_zap.b00  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a506_zap.b01  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a506_zap.b02  ${FIRMWARE_INSTALL_DIR}

}

do_install_append_apq8017(){

   cp -rf ${S}/include/private/C2D/c2d2.h       ${D}${includedir}
   cp -rf ${S}/include/private/C2D/c2d2.h       ${D}${includedir}/adreno
   cp -rf ${S}/c2d30/include/c2dExt.h           ${D}${includedir}
   cp -rf ${S}/c2d30/include/c2dExt.h           ${D}${includedir}/adreno

   #Install C2D libs
   install_c2d_libs

   #Deciding firmware installation directory
   if [ "${MLPREFIX}" == "lib32-" ] || [ "${MLPREFIX}" == "" -a "${TUNE_ARCH}" == "arm" ]; then
       export FIRMWARE_INSTALL_DIR=${D}/lib/firmware
   else
       install -d ${D}/lib64/firmware
       export  FIRMWARE_INSTALL_DIR=${D}/lib64/firmware
   fi
   # A3X-only firmware
   install -m 0644 ${WORKDIR}/build/firmware/a300_pfp.fw  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a300_pm4.fw  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a330_pfp.fw  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a330_pm4.fw  ${FIRMWARE_INSTALL_DIR}
}

do_install_append_apq8096(){

   cp -rf ${S}/include/private/C2D/c2d2.h       ${D}${includedir}
   cp -rf ${S}/include/private/C2D/c2d2.h       ${D}${includedir}/adreno
   cp -rf ${S}/c2d30/include/c2dExt.h           ${D}${includedir}
   cp -rf ${S}/c2d30/include/c2dExt.h           ${D}${includedir}/adreno

   #Install C2D libs
   install_c2d_libs

   #Deciding firmware installation directory
   if [ "${MLPREFIX}" == "lib32-" ] || [ "${MLPREFIX}" == "" -a "${TUNE_ARCH}" == "arm" ]; then
       export FIRMWARE_INSTALL_DIR=${D}/lib/firmware
   else
       install -d ${D}/lib64/firmware
       export  FIRMWARE_INSTALL_DIR=${D}/lib64/firmware
   fi

   # currently, all A5X chips use the A530 CP firmware
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530_pfp.fw   ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530_pm4.fw   ${FIRMWARE_INSTALL_DIR}

   # A530-only firmware
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530v1_pfp.fw ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530v1_pm4.fw ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530_zap.elf  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530_zap.mdt  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530_zap.b00  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530_zap.b01  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530_zap.b02  ${FIRMWARE_INSTALL_DIR}

   install -m 0644 ${WORKDIR}/adreno200/firmware/a5x/a530_gpmu.fw2   ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/adreno200/firmware/a5x/a530v3_gpmu.fw2 ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/adreno200/firmware/a5x/a530v2_seq.fw2  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/adreno200/firmware/a5x/a530v3_seq.fw2  ${FIRMWARE_INSTALL_DIR}
}

do_install_append_apq8098(){

   # A540-only firmware
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a540_zap.elf  ${D}/lib/firmware/
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a540_zap.mdt  ${D}/lib/firmware/
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a540_zap.b00  ${D}/lib/firmware/
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a540_zap.b01  ${D}/lib/firmware/
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a540_zap.b02  ${D}/lib/firmware/

   # currently, all A5X chips use the A530 CP firmware
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530_pfp.fw   ${D}/lib/firmware/
   install -m 0644 ${WORKDIR}/build/firmware/a5x/a530_pm4.fw   ${D}/lib/firmware/

   install -m 0644 ${WORKDIR}/adreno200/firmware/a5x/a540_gpmu.fw2   ${D}/lib/firmware/

   mkdir -p ${STAGING_DIR_TARGET}/egl-wayland-subdriver
   mkdir -p ${STAGING_DIR_TARGET}/egl-wayland-subdriver/eglWaylandSubdriver
   mkdir -p ${STAGING_DIR_TARGET}/egl-wayland-subdriver/eglWaylandSubdriver/native
   cp -rf ${SRC_DIR}/opengl/esx/eglsubdrivers/wayland/* ${STAGING_DIR_TARGET}/egl-wayland-subdriver/eglWaylandSubdriver/
   cp -rf ${SRC_DIR}/opengl/esx/egl/native/*            ${STAGING_DIR_TARGET}/egl-wayland-subdriver/eglWaylandSubdriver/native/
   cp ${THISDIR}/egl-wayland-subdriver.bb               ${STAGING_DIR_TARGET}/egl-wayland-subdriver/
   cp ${THISDIR}/CMakeLists.txt                         ${STAGING_DIR_TARGET}/egl-wayland-subdriver/
   cd ${STAGING_DIR_TARGET}/egl-wayland-subdriver
   zip -r egl-wayland-subdriver.zip .
   cd ${WORKDIR}/build
   mv ${STAGING_DIR_TARGET}/egl-wayland-subdriver/egl-wayland-subdriver.zip ${D}${libdir}
   rm -rf ${STAGING_DIR_TARGET}/egl-wayland-subdriver
}

do_install_append_qcs605(){

   cp -rf ${S}/include/private/C2D/c2d2.h       ${D}${includedir}
   cp -rf ${S}/include/private/C2D/c2d2.h       ${D}${includedir}/adreno
   cp -rf ${S}/c2d30/include/c2dExt.h           ${D}${includedir}
   cp -rf ${S}/c2d30/include/c2dExt.h           ${D}${includedir}/adreno

   #Install C2D libs
   install_c2d_libs

   #Deciding firmware installation directory
   if [ "${MLPREFIX}" == "lib32-" ] || [ "${MLPREFIX}" == "" -a "${TUNE_ARCH}" == "arm" ]; then
       export FIRMWARE_INSTALL_DIR=${D}/lib/firmware
   else
       install -d ${D}/lib64/firmware
       export  FIRMWARE_INSTALL_DIR=${D}/lib64/firmware
   fi

   # A630 common firmware file
   install -m 0644 ${WORKDIR}/build/firmware/a6x/a630_gmu.bin  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a6x/a630_sqe.fw  ${FIRMWARE_INSTALL_DIR}

   # A615-only firmware
   install -m 0644 ${WORKDIR}/build/firmware/a6x/a615_sqe.fw  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a6x/a615_zap.elf  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a6x/a615_zap.mdt  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a6x/a615_zap.b00  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a6x/a615_zap.b01  ${FIRMWARE_INSTALL_DIR}
   install -m 0644 ${WORKDIR}/build/firmware/a6x/a615_zap.b02  ${FIRMWARE_INSTALL_DIR}
}

FILES_${PN} = "${includedir}/* \
               ${nonarch_base_libdir}/firmware/* \
               ${nonarch_libdir}/* \
             ${base_libdir}/firmware/* \
             ${bindir}/* \
             ${libdir}/* "

INSANE_SKIP_${PN} += "dev-so arch"

FILES_${PN}-dev = ""
