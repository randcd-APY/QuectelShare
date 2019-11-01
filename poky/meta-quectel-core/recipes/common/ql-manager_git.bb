inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel log system"
PR = "r7"

DEPENDS = "libnl ql-common-tools data diag dsutils glib-2.0 qmi qmi-framework"

SRC_DIR = "${WORKSPACE}/quectel-core/ql-manager/"
S = "${WORKDIR}/quectel-core/ql-manager"

FILES_${PN} += "/lib/systemd/*"

PACKAGES = "${PN}"
FILES_${PN} += "${libdir}/*"
FILES_${PN}-dbg += "${libdir}/.debug/*"
FILES_${PN} += "${includedir}/*"
INSANE_SKIP_${PN} = "dev-so"
do_install() {
       install -m 0755 ${WORKDIR}/quectel-core/ql-manager/etc/start_ql_manager_server_le -D ${D}${sysconfdir}/init.d/start_ql_manager_server_le
       install -m 0755 ${WORKDIR}/quectel-core/ql-manager/etc/start_emac_le -D ${D}${sysconfdir}/init.d/start_emac_le

       install -d ${D}${systemd_unitdir}/system
       install -m 0644 ${WORKDIR}/quectel-core/ql-manager/etc/ql_manager_server.service.in \
       ${D}${systemd_unitdir}/system/ql_manager_server.service

       install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
       ln -sf ${systemd_unitdir}/system/ql_manager_server.service \
       ${D}${systemd_unitdir}/system/multi-user.target.wants/ql_manager_server.service

       install -d ${D}${bindir}
       install -m 0755 bin/* ${D}${bindir}

       install -d ${D}${libdir}
       install -m 0755 lib/* ${D}${libdir}
}

do_compile() {
}

do_package_qa() {
}
