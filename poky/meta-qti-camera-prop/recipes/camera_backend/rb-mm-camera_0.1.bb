inherit autotools pkgconfig qlicense qprebuilt

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/services/mm-camera-legacy/"
SRC_URI  += "file://mm-qcamera-daemon.service"
SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/camera/services/mm-camera-legacy/"

SRC_DIR = "${WORKSPACE}/camera/services/mm-camera-legacy/"

DEPENDS += "glib-2.0 libxml2 adreno200 rb-camera rb-mm-camera-lib rb-mm-3a-core rb-mm-camera-core"
DEPENDS_append_quec-smart += "libui"

do_package_qa[noexec] = "1"

CFLAGS += "-Wno-error -Wno-uninitialized -Wno-error=attributes -Wno-error=unused-parameter"
CFLAGS += "-Wno-error=builtin-macro-redefined -Wno-error=type-limits"
CFLAGS += "-D__unused="__attribute__((__unused__))""
CFLAGS += "-D_GNU_SOURCE"
CFLAGS += "-DUNIX_PATH_MAX=108"
CFLAGS += "-DIOT_PLATFORM"
CFLAGS += "-D_LE_CAMERA_"
CFLAGS += "-I${STAGING_KERNEL_BUILDDIR}/usr/include/"
CFLAGS += "-I${STAGING_INCDIR}/adreno/"
CFLAGS += "-I${STAGING_INCDIR}/mm-camera-legacy/"

CFLAGS += "-I${STAGING_INCDIR}/glib-2.0"
CFLAGS += "-I${STAGING_LIBDIR}/glib-2.0/include"

CFLAGS += "-include linux/limits.h"
CFLAGS += "-include stddef.h"
CFLAGS += "-include stdint.h"
CFLAGS += "-Dstrlcpy=g_strlcpy"
CFLAGS += "-Dstrlcat=g_strlcat"
CFLAGS += "-include glib.h"
CFLAGS += "-include glibconfig.h"
CFLAGS += "-include sys/ioctl.h"
CFLAGS += "-include time.h"
CFLAGS += "-include sys/time.h"

CFLAGS_append_robot-rome += "-DROBOT_ROME"
CFLAGS_append_robot-pronto += "-DROBOT_PRONTO"

LDFLAGS += "-lcutils"
LDFLAGS += "-lglib-2.0"
LDFLAGS += "-llog"
LDFLAGS += "-lrt"
LDFLAGS += "-lxml2"
LDFLAGS += "-lz"
LDFLAGS += "-ldl"
LDFLAGS += "-lmmcamera2_is"

def get_robot_target(d):
    if d.getVar('BASEMACHINE', True) == 'msm8909-robot':
        return "ROBOT_TARGET=true"
    else:
        return ""

EXTRA_OECONF = "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
               --with-glib \
               --with-common-includes=${STAGING_INCDIR} \
               --with-camera-hal-path=${WORKSPACE}/camera/lib-legacy \
               --with-mm-camerasdk-path=${WORKSPACE}/camera/services/mm-camerasdk \
               --with-mm-camera-lib-path=${WORKSPACE}/camera/services/mm-camera-lib-legacy \
               --with-xml2-includes=${STAGING_INCDIR}/libxml2"
EXTRA_OECONF += "${@get_robot_target(d)}"

include rb-mm-camera-msm8909.inc

FILES_${PN}-dbg  = "${libdir}/.debug/* /system/bin/.debug/* "
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/* ${bindir}/* \
                   /lib/firmware/* /system/* /data/* /lib/systemd/system/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
INSANE_SKIP_${PN} = "dev-so"
INSANE_SKIP_${PN} += "installed-vs-shipped"
INSANE_SKIP_${PN} += "textrel"
INSANE_SKIP_${PN} += "already-stripped"
INSANE_SKIP_${PN} += "ldflags"
INSANE_SKIP_${PN} += "staticdev"
