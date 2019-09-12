inherit qcommon qlicense systemd update-rc.d qprebuilt

DESCRIPTION = "Thermal Engine"
SECTION = "base"

PR = "r11"

FILESPATH =+ "${WORKSPACE}:"
S = "${WORKDIR}/thermal-engine"
SRC_URI =+ "file://thermal-engine.service"

SRC_DIR = "${WORKSPACE}/thermal-engine"

DEPENDS = "qmi-framework glib-2.0"

EXTRA_OECONF  = "--with-qmi-framework  --with-glib"
EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF += "--enable-target-${BASEMACHINE}=yes"
EXTRA_OECONF += "KERNEL_VER=${PREFERRED_VERSION_linux-msm}"

INITSCRIPT_NAME = "thermal-engine"
INITSCRIPT_PARAMS = "start 40 2 3 4 5 . stop 60 0 1 6 ."

FILES_${PN} += "/lib/systemd/*"
FILES_${PN} += "/etc/systemd/*"

do_install_append() {
       if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
           install -d ${D}${sysconfdir}/systemd/system/
           install -m 0644 ${WORKDIR}/thermal-engine.service -D \
                ${D}${sysconfdir}/systemd/system/thermal-engine.service
           install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
           ln -sf /etc/systemd/system/thermal-engine.service \
                ${D}/etc/systemd/system/multi-user.target.wants/thermal-engine.service
	   install -d ${D}/lib/systemd/system/ffbm.target.wants/
	   ln -sf /etc/systemd/system/thermal-engine.service \ 
		${D}/lib/systemd/system/ffbm.target.wants/thermal-engine.service
       else
           install -m 0755 ${WORKDIR}/thermal-engine/start_thermal-engine_le -D \
                ${D}${sysconfdir}/init.d/thermal-engine
       fi
}
do_configure[depends] += "virtual/kernel:do_shared_workdir"
