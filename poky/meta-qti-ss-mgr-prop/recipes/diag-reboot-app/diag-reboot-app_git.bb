inherit qcommon qlicense qprebuilt

DESCRIPTION = "Reboot Diag"
PR = "r5"
DEPENDS = "glib-2.0 diag qmi"

SRC_DIR = "${WORKSPACE}/mdm-ss-mgr/diag-reboot-app/"

FILESPATH =+ "${WORKSPACE}/:"
SRC_URI = "file://mdm-ss-mgr/diag-reboot-app/"
SRC_URI += "file://diag-reboot-app.service"

S = "${WORKDIR}/mdm-ss-mgr/diag-reboot-app/"

EXTRA_OECONF += "--with-glib --with-common-includes=${STAGING_INCDIR}"
FILES_${PN} += "${systemd_unitdir}/system/"

do_install_append() {
    #install -m 0755 ${S}diag-reboot-app -D ${D}/usr/bin/diagrebootapp
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
      install -d ${D}${systemd_unitdir}/system/
      install -m 0644 ${WORKDIR}/diag-reboot-app.service -D ${D}${systemd_unitdir}/system/diag-reboot-app.service
      install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
      install -d ${D}${systemd_unitdir}/system/ffbm.target.wants/
      # enable the service for multi-user.target
      ln -sf ${systemd_unitdir}/system/diag-reboot-app.service \
           ${D}${systemd_unitdir}/system/multi-user.target.wants/diag-reboot-app.service
      # enable the service for ffbm.target
      ln -sf ${systemd_unitdir}/system/diag-reboot-app.service \
           ${D}${systemd_unitdir}/system/ffbm.target.wants/diag-reboot-app.service
   fi
}
