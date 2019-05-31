inherit autotools qcommon qlicense qprebuilt
DESCRIPTION = "tftp_server server module"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"

SRC_URI = "file://tftp/"

SRC_DIR = "${WORKSPACE}/tftp"

S = "${WORKDIR}/tftp"

DEPENDS += "glib-2.0"
DEPENDS += "common"
DEPENDS += "virtual/kernel"

EXTRA_OECONF_append = " --with-glib"
EXTRA_OECONF_append = " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"

do_install() {
   install -m 0755 ${S}/tftp_server -D ${D}/sbin/tftp_server
   install -m 0755 ${S}/tftp_server.sh -D ${D}${sysconfdir}/init.d/tftp_server.sh
}

do_install_append() {

   # For Ram dumps
   install -m 0700 -d ${D}/data/tombstones/modem
   install -m 0700 -d ${D}/data/tombstones/lpass

   #######################################################################

   #HLOS shared folders
   install -m 0770 -d ${D}/data/persist/hlos_rfs/shared/

   #######################################################################

   #The mdm folder structure
   install -m 0700 -d ${D}/system/rfs/mdm/mpss/readonly
   install -m 0700 -d ${D}/system/rfs/mdm/adsp/readonly

   install -m 0700 -d ${D}/data/persist/rfs/mdm/mpss
   install -m 0700 -d ${D}/data/persist/rfs/mdm/adsp
   install -m 0700 -d ${D}/data/persist/rfs/shared


   ln -s /data/tombstones/modem ${D}/system/rfs/mdm/mpss/ramdumps
   ln -s /data/persist/rfs/mdm/mpss ${D}/system/rfs/mdm/mpss/readwrite
   ln -s /data/persist/rfs/shared ${D}/system/rfs/mdm/mpss/shared
   ln -s /data/persist/hlos_rfs/shared ${D}/system/rfs/mdm/mpss/hlos
   ln -s /firmware ${D}/system/rfs/mdm/mpss/readonly/firmware

   ln -s /data/tombstones/lpass ${D}/system/rfs/mdm/adsp/ramdumps
   ln -s /data/persist/rfs/mdm/adsp ${D}/system/rfs/mdm/adsp/readwrite
   ln -s /data/persist/rfs/shared ${D}/system/rfs/mdm/adsp/shared
   ln -s /data/persist/hlos_rfs/shared ${D}/system/rfs/mdm/adsp/hlos
   ln -s /firmware ${D}/system/rfs/mdm/adsp/readonly/firmware

   #######################################################################

   #The msm folder structure
   install -m 0700 -d ${D}/system/rfs/msm/mpss/readonly
   install -m 0700 -d ${D}/system/rfs/msm/adsp/readonly

   install -m 0700 -d ${D}/data/persist/rfs/msm/mpss
   install -m 0700 -d ${D}/data/persist/rfs/msm/adsp
   install -m 0700 -d ${D}/data/persist/rfs/shared


   ln -s /data/tombstones/modem ${D}/system/rfs/msm/mpss/ramdumps
   ln -s /data/persist/rfs/msm/mpss ${D}/system/rfs/msm/mpss/readwrite
   ln -s /data/persist/rfs/shared ${D}/system/rfs/msm/mpss/shared
   ln -s /data/persist/hlos_rfs/shared ${D}/system/rfs/msm/mpss/hlos
   ln -s /firmware ${D}/system/rfs/msm/mpss/readonly/firmware

   ln -s /data/tombstones/lpass ${D}/system/rfs/msm/adsp/ramdumps
   ln -s /data/persist/rfs/msm/adsp ${D}/system/rfs/msm/adsp/readwrite
   ln -s /data/persist/rfs/shared ${D}/system/rfs/msm/adsp/shared
   ln -s /data/persist/hlos_rfs/shared ${D}/system/rfs/msm/adsp/hlos
   ln -s /firmware ${D}/system/rfs/msm/adsp/readonly/firmware
}

FILES_${PN} += "/system /data"

inherit update-rc.d

INITSCRIPT_NAME = "tftp_server.sh"
INITSCRIPT_PARAMS = "start 20 2 3 4 5 ."

pkg_postinst_${PN} () {
        [ -n "$D" ] && OPT="-r $D" || OPT="-s"
        # remove all rc.d-links potentially created from alternatives
        update-rc.d $OPT -f ${INITSCRIPT_NAME} remove
        update-rc.d $OPT ${INITSCRIPT_NAME} ${INITSCRIPT_PARAMS}
}
do_configure[depends] += "virtual/kernel:do_shared_workdir"
