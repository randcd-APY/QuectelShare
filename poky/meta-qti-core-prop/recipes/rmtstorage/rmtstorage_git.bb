inherit autotools qcommon qprebuilt qlicense systemd

DESCRIPTION = "rmt_storage server module"
PR = "r10"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI  = "file://remotefs/"
SRC_URI += "file://rmt_storage.sh"
SRC_URI += "file://rmt_storage.service"

SRC_DIR = "${WORKSPACE}/remotefs"

S = "${WORKDIR}/remotefs"

DEPENDS += "glib-2.0 qmi qmi-framework virtual/kernel libcutils"

EXTRA_OECONF_append = " --with-glib"
EXTRA_OECONF_append = " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"

CFLAGS += "-I${STAGING_INCDIR}/cutils"

do_install() {
   install -m 0755 ${S}/rmt_storage -D ${D}/sbin/rmt_storage
   if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
       install -d ${D}${systemd_unitdir}/system/
       install -m 0644 ${WORKDIR}/rmt_storage.service -D ${D}${systemd_unitdir}/system/rmt_storage.service
       install -d ${D}${systemd_unitdir}/system/sysinit.target.wants/
       ln -sf ${systemd_unitdir}/system/rmt_storage.service \
            ${D}${systemd_unitdir}/system/sysinit.target.wants/rmt_storage.service
   fi
}

FILES_${PN} += "${systemd_unitdir}/system/"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
