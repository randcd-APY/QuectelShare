inherit qcommon qprebuilt qlicense autotools-brokensep

DESCRIPTION = "sensors-see common system interface library"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

DEPENDS = "liblog"
DEPENDS += "libcutils"
DEPENDS += "libutils"
DEPENDS += "system-core"
DEPENDS += "common-headers"
DEPENDS += "glib-2.0"
DEPENDS += "qmi-framework"
DEPENDS += "diag"
DEPENDS += "libhardware"
DEPENDS += "protobuf"
DEPENDS += "protobuf-native"

FILESPATH =+ "${WORKSPACE}/vendor/qcom/proprietary/commonsys-intf/:"
SRC_URI = "file://sensors-see/"

S = "${WORKDIR}/sensors-see"

SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/commonsys-intf/sensors-see"

CFLAGS += "-I${STAGING_INCDIR}/cutils"
CFLAGS += "-I${STAGING_INCDIR}/utils"
CFLAGS += "-I${STAGING_LIBDIR}/glib-2.0/include"
CPPFLAGS += "-DGOOGLE_PROTOBUF_NO_RTTI"

EXTRA_OECONF += " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF += " --with-glib"
EXTRA_OECONF += " --with-common-includes=${STAGING_INCDIR}"
EXTRA_OECONF += " --enable-sns-le-qcs605"

EXTRA_OEMAKE += " 'CC=${CC}' 'CFLAGS=${CFLAGS}'"

do_install_append() {
    install -d ${D}${sysconfdir}/sensors/proto
    install -m 0644 ${S}/ssc/proto/*.proto -D ${D}${sysconfdir}/sensors/proto
    install -m 0644 ${STAGING_INCDIR_NATIVE}/google/protobuf/descriptor.proto -D ${D}${sysconfdir}/sensors/proto
}

#Disable the split of debug information into -dbg files
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

#Skips check for .so symlinks
INSANE_SKIP_${PN} = "dev-so"

FILES_${PN} += "${includedir}/*"
FILES_${PN} += "/usr/lib/*"
FILES_${PN} += "/usr/lib64/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
FILES_${PN} += "${sysconfdir}/sensors/*"

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
do_configure[depends] += "virtual/kernel:do_shared_workdir"
