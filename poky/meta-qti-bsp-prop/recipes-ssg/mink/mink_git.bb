inherit qcommon qlicense qprebuilt

DESCRIPTION = "mink"

RDEPENDS_${PN} = "libcutils"
COMPATIBLE_HOST = "(arm|aarch64).*-linux"
FILESPATH =+ "${WORKSPACE}:"

SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/ssg/mink"
S = "${WORKDIR}/vendor/qcom/proprietary/ssg/mink"
PR = "0"

EXTRA_OEMAKE += "ARCH=${TARGET_ARCH} CROSS_COMPILE=${TARGET_PREFIX}"

FILES_${PN} += "${libdir} ${includedir}"
