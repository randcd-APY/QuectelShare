FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += "\
            file://find-touchscreen.sh \
            file://automountsdcard.sh \
            file://usb.sh \
            file://mdev.conf \
            file://profile \
            file://fstab \
            file://inittab \
            file://rcS \
            file://no-console.cfg \
            file://login.cfg \
            file://mdev.cfg \
            file://base.cfg \
            file://syslog-startup.conf \
            file://busybox-syslog.service \
            file://busybox_klogd.patch;patchdir=.. \
            file://iio.sh \
            file://0001-Support-MTP-function.patch \
            file://fix-mdev-crash.patch \
            file://fix_uninitialized_memory.patch \
"
SRC_URI_append_apq8053 += "file://apq8053/mdev.conf"
SRC_URI_append_mdm9607 += "file://mdm9607/mdev.conf"
SRC_URI_append_mdm9607 += "file://mdm9607/sensors.sh"
SRC_URI_append += "${@bb.utils.contains('DISTRO_FEATURES', 'virtualization', 'file://0001-Remove-readprofile-and-brctl-from-busybox.links-file.patch', '', d)}"

prefix = ""

BUSYBOX_SPLIT_SUID = "0"

FILES_${PN} += "/usr/bin/env"

do_compile_append_mdm() {
    sed -i '/modprobe/d' ./busybox.links
}

do_install_append() {
    # systemd is udev compatible.
    if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
        install -d ${D}${sysconfdir}/udev/scripts/
        install -m 0744 ${WORKDIR}/automountsdcard.sh \
            ${D}${sysconfdir}/udev/scripts/automountsdcard.sh
        install -d ${D}${systemd_unitdir}/system/
        install -m 0644 ${WORKDIR}/busybox-syslog.service -D ${D}${systemd_unitdir}/system/busybox-syslog.service
        install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
        # enable the service for multi-user.target
        ln -sf ${systemd_unitdir}/system/busybox-syslog.service \
           ${D}${systemd_unitdir}/system/multi-user.target.wants/busybox-syslog.service
        install -d ${D}${sysconfdir}/initscripts
        install -m 0755 ${WORKDIR}/syslog ${D}${sysconfdir}/initscripts/syslog
        sed -i 's/syslogd -- -n/syslogd -n/' ${D}${sysconfdir}/initscripts/syslog
        sed -i 's/init.d/initscripts/g'  ${D}${systemd_unitdir}/system/busybox-syslog.service
    else
        install -d ${D}${sysconfdir}/mdev
        install -m 0755 ${WORKDIR}/automountsdcard.sh ${D}${sysconfdir}/mdev/
        install -m 0755 ${WORKDIR}/find-touchscreen.sh ${D}${sysconfdir}/mdev/
        install -m 0755 ${WORKDIR}/usb.sh ${D}${sysconfdir}/mdev/
        install -m 0755 ${WORKDIR}/iio.sh ${D}${sysconfdir}/mdev/
    fi
    if [ ${BASEMACHINE} == "mdm9607" ]; then
     install -m 0755 ${WORKDIR}/mdm9607/sensors.sh ${D}${sysconfdir}/mdev/
    elif [ ${BASEMACHINE} == "apq8053"];then
     install -m 0644 ${WORKDIR}/apq8053/mdev.conf ${D}${sysconfdir}/
    fi
    chmod -R go-x ${D}${sysconfdir}/mdev/
    mkdir -p ${D}/usr/bin
    ln -s /bin/env ${D}/usr/bin/env
}

# util-linux installs dmesg with priority 80. Use higher priority than util-linux to get busybox dmesg installed.
ALTERNATIVE_PRIORITY[dmesg] = "100"

#FILES_${PN}-mdev += "${sysconfdir}/mdev/* "
