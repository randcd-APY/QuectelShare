inherit autotools pkgconfig qlicense sdllvm qprebuilt

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/services/mm-camera-lib/"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/camera/services/mm-camera-lib/"

SRC_DIR   = "${WORKSPACE}/camera/services/mm-camera-lib/"

def get_fastcv_depends(d):
    if d.getVar('BASEMACHINE', True) == 'apq8098':
        return ""
    else:
        return "fastcv-noship"

DEPENDS += "glib-2.0 libxml2 cameradbg"
DEPENDS += "${@get_fastcv_depends(d)}"

ENABLE_SDLLVM = "true"
TARGET_CFLAGS +="${THUMB_FLAGS} ${OPTIONAL_CFLAGS}"
TARGET_LDFLAGS +="${OPTIONAL_LDFLAGS} -avoid-version"

def get_platform(d):
    if d.getVar('BASEMACHINE', True) == 'apq8053':
        return "TARGET_BOARD_PLATFORM=8953"
    elif d.getVar('BASEMACHINE', True) == 'apq8096':
        return "TARGET_BOARD_PLATFORM=8996"
    elif d.getVar('BASEMACHINE', True) == 'apq8098':
        return "TARGET_BOARD_PLATFORM=8998"
    else:
        return ""

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                --with-glib \
                --with-common-includes=${STAGING_INCDIR} \
                --with-camera-hal-path=${WORKSPACE}/camera/lib \
                --with-mm-camera-path=${WORKSPACE}/camera/services/mm-camera/mm-camera2 \
                --with-camera-core=${WORKSPACE}/camera/services/mm-camera-core \
                --with-3a-core=${WORKSPACE}/camera/services/mm-3a-core \
                --with-mm-camerasdk=${WORKSPACE}/camera/services/mm-camerasdk"

include ${BASEMACHINE}.inc
EXTRA_OECONF += "${@get_platform(d)}"
EXTRA_OECONF += "TARGET_NEON_ENABLED=true"
EXTRA_OECONF += "COMPILE_MMCAMERA2=true"
EXTRA_OECONF += "USE_C_FUSION=true"
EXTRA_OECONF += "TARGET_ARM32=true"
EXTRA_OECONF += "CAC3_DEBUG_LOGS_ENABLED=true"
EXTRA_OECONF += "GPU_USE_NEW_OPENCL_API=true"

do_install_append(){

    #for bayer_proc prebuilt binaries
    binaries_out="${D}/system/etc/camera"
    mkdir -p $binaries_out

    bayer_proc_binary="shdr-1x1.bin"
    bayer_proc_binary_location="${S}/bayer_proc/prebuilt/svhdr/bin64"
    if [ "${MACHINE}" == "apq8053-32" ]; then
        bayer_proc_binary_location="${S}/bayer_proc/prebuilt/svhdr/bin32"
    fi
    if [ -f "$bayer_proc_binary_location/$bayer_proc_binary" ];then
        cp $bayer_proc_binary_location/$bayer_proc_binary $binaries_out
    fi

    if [ "${BASEMACHINE}" == "apq8096" ]; then

       libs_location="${S}/hvx/8996"
       libs_out="${D}/usr/lib"
       mkdir -p $libs_out
       if [ -d "$libs_location" ];then
          cp $libs_location/*.so $libs_out
       fi
    fi

    if [ "${BASEMACHINE}" == "apq8098" ]; then

       libs_location="${S}/hvx/8998"
       libs_out="${D}/usr/lib"
       mkdir -p $libs_out
       if [ -d "$libs_location" ];then
          cp $libs_location/*.so $libs_out
       fi
    fi
    cp ${S}/bc/prebuilt/LE/usr/lib/libBC.so ${D}/usr/lib/
    cp ${S}/llvd/prebuilt/llvd_sm/lib/libseemore_le.so ${D}/usr/lib/
    cp ${S}/bayer_proc/prebuilt/svhdr/lib/libShdr_LE.so ${D}/usr/lib/libShdr.so
}

#For cp/prebuilt 32 bit libraries
do_install_append_arm() {
    libs="libubifocus.so libchromaflash.so liboptizoom.so 32/libtrueportrait.so 32/libdualcameraddm.so libtruescanner.so libblurbuster.so libseestraight.so 32/libclearsight.so 32/libfiltergenerator.so"
    libs_location="${S}/cp/prebuilt"
    libs_out="${D}/usr/lib"

    mkdir -p $libs_out
    for i in $libs; do
        if [ -f "$libs_location/$i" ];then
            cp $libs_location/$i $libs_out
        fi
    done

    if [ -f "$libs_location/libhazebuster_le.so" ]; then
         cp $libs_location/libhazebuster_le.so $libs_out/libhazebuster.so
    fi
}

def get_8098_install_skip(d):
    if d.getVar('BASEMACHINE', True) == 'apq8098':
        return "already-stripped stripped staticdev ldflags build-deps file-rdeps"

def get_8098_debug(d):
    if d.getVar('BASEMACHINE', True) == 'apq8098':
        return "/usr/bin/.debug/*"

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
PACKAGES = "${PN}-dbg ${PN} ${PN}-dev"
#FILES_${PN}-dbg  += "${libdir}/.debug/*"
#FILES_${PN} += "${libdir} /system/etc/* "
#FILES_${PN}-dev += "${ includedir }"

#PACKAGES = "${PN}"
FILES_${PN}      = "${libdir}"
FILES_${PN}     += "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/* /system/etc/* "
FILES_${PN}-dbg  = "${libdir}/.debug/* ${@get_8098_debug(d)}"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
INSANE_SKIP_${PN} = "dev-so"
INSANE_SKIP_${PN} += "arch"
INSANE_SKIP_${PN} += "${@get_8098_install_skip(d)}"
