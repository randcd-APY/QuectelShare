FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

# Only add selinux config if selinux feature is enabled and only on userdebug
DEPENDS += "${@base_conditional('PERF_BUILD', '1', '', bb.utils.contains('DISTRO_FEATURES', 'selinux', 'libselinux', '', d), d)}"
SRC_URI += "${@base_conditional('PERF_BUILD', '1', '', bb.utils.contains('DISTRO_FEATURES', 'selinux', 'file://selinux.cfg', '', d), d)}"
