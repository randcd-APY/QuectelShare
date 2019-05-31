inherit autotools pkgconfig qlicense sdllvm qprebuilt

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/services/mm-camera/"
SRC_URI  += "file://mm-qcamera-daemon.service"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/camera/services/mm-camera/mm-camera2/"

SRC_DIR = "${WORKSPACE}/camera/services/mm-camera/mm-camera2/"

def get_depends(bb, d):
    srcdir = d.getVar('SRC_DIR', True)
    if (os.path.isdir(srcdir)):
        return "cameradbg mm-3a-core mm-camerasdk"
    else:
        return "cameradbg mm-3a-core"

def get_adreno_depends(d):
    if d.getVar('BASEMACHINE', True) == 'apq8098':
        return ""
    else:
        return "adreno200"

DEST_DIR_FLIGHT_ADDON = "${WORKSPACE}/FEAT-API-LNX-IMU/"
def get_flight_addon_depends(d):
    dest_flightaddon = d.getVar('DEST_DIR_FLIGHT_ADDON', True)
    if (os.path.isdir(dest_flightaddon)):
        return ""
    elif d.getVar('BASEMACHINE', True) == 'apq8096' and d.getVar('PRODUCT', True) == 'drone':
        return "flight-addon"
    else:
        return ""

DEST_DIR_DRONESPREBUILTS = "${WORKSPACE}/FEAT-API-LNX-DG/"
def get_dronesprebuilts_depends(d):
    dest_dronesprebuilts = d.getVar('DEST_DIR_DRONESPREBUILTS', True)
    if (os.path.isdir(dest_dronesprebuilts)):
        return ""
    elif d.getVar('BASEMACHINE', True) == 'apq8096' and d.getVar('PRODUCT', True) == 'drone':
        return "dronesprebuilts"
    else:
        return ""

DEPENDS += "glib-2.0 libxml2  camera mm-camera-noship mm-camera-lib ${@get_depends(bb, d)}"
DEPENDS += "${@get_adreno_depends(d)}"
DEPENDS += "${@get_flight_addon_depends(d)}"
DEPENDS += "${@get_dronesprebuilts_depends(d)}"

ENABLE_SDLLVM = "true"
TARGET_CFLAGS +="${THUMB_FLAGS} ${OPTIONAL_CFLAGS}"
TARGET_LDFLAGS +="${OPTIONAL_LDFLAGS} -avoid-version"

#Start unpack only after cameradbg packaging finishes
do_unpack[depends] += "${MLPREFIX}cameradbg:do_packagedata"

def config_camera_debug_data(d):
    if d.getVar('BASEMACHINE', True) == 'apq8096' and d.getVar('PRODUCT', True) == 'drone':
        return "CAMERA_DEBUG_DATA=true"
    else:
        return ""

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
               --with-glib \
               --with-common-includes=${STAGING_INCDIR} \
               --with-camera-hal-path=${WORKSPACE}/camera/lib \
               --with-mm-camerasdk-path=${WORKSPACE}/camera/services/mm-camerasdk \
               --with-mm-camera-lib-path=${WORKSPACE}/camera/services/mm-camera-lib \
               --with-xml2-includes=${STAGING_INCDIR}/libxml2"
EXTRA_OECONF += "${@config_camera_debug_data(d)}"

include ${BASEMACHINE}.inc
include mm-camera-${BASEMACHINE}.inc

## This recipe builds many sensors and actuators by default. Among them only one is used
## in device. To avoid rootfs cluttering, package these into seperate packages namely
## ${PN}-essential, ${PN}-optional and install judiciously.

SENSOR_SUPPORTED="imx274 imx477"
ACTUATOR_SUPPORTED="none"
PACKAGES = "${PN}-essential ${PN}-optional ${PN}"

def get_essential_libs(bb, d):
    libs_list = ""
    # Add sensor specific chromatix libs.
    for sensor in d.getVar("SENSOR_SUPPORTED", True).split():
        libs_list += "${libdir}/libchromatix_"+sensor+"*.* "
        libs_list += "${libdir}/libmmcamera_"+sensor+"*.* "
        libs_list += "/system/etc/camera/"+sensor+"_chromatix*.xml "

    # Add actuator libs to essential package.
    for actuator in d.getVar("ACTUATOR_SUPPORTED", True).split():
        libs_list += "${libdir}/libactuator_"+actuator+"*.* "

    return libs_list

FILES_${PN}-essential = "${@get_essential_libs(bb, d)}"

def get_8098_files(d):
    if d.getVar('BASEMACHINE', True) == 'apq8098':
        return "/system/* /usr/include/* /lib/firmware/* ${libdir}/* /data/* ${systemd_unitdir}/system/ /usr/bin/*"

def get_8098_install_skip(d):
    if d.getVar('BASEMACHINE', True) == 'apq8098':
        return "installed-vs-shipped already-stripped ldflags staticdev debug-files"

def get_8098_debug(d):
    if d.getVar('BASEMACHINE', True) == 'apq8098':
        return "/usr/bin/.debug/*"

# Add remaining chromatix libs to optional package.
FILES_${PN}-dbg  = "${libdir}/.debug/* ${@get_8098_debug(d)}"

FILES_${PN} = "${@get_8098_files(d)}"

FILES_${PN} += "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/* \
                   /lib/firmware/* /system/bin/mm-qcamera-daemon /system/etc/* /data/* ${systemd_unitdir}/system/"

FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"

FILES_${PN}-optional  = "${libdir}/libchromatix_*.* ${libdir}/libactuator_*.* ${libdir}/libmmcamera_imx*.* ${libdir}/libmmcamera_ov*.* ${libdir}/libmmcamera_s5k*.* /system/etc/camera/*_chromatix*.xml"

INSANE_SKIP_${PN} = "dev-so"
INSANE_SKIP_${PN}-optional = "dev-so"
INSANE_SKIP_${PN}-essential = "dev-so"
INSANE_SKIP_${PN} += "${@get_8098_install_skip(d)}"
