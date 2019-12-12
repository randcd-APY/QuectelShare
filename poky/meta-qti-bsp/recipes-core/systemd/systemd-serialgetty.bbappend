inherit qperf

FILESEXTRAPATHS_prepend := "${THISDIR}/systemd-serialgetty:"

SRC_URI_remove = "file://serial-getty@.service"
SRC_URI_append = " file://serial-getty@ttyDEV.service"

SERIAL_CONSOLES ?= "ttyHSL0"

# Override default do_install, QTI targets support only one baudrate per ttydevice
do_install() {
    if [ ! -z "${SERIAL_CONSOLES}" ] ; then
        tmp="${SERIAL_CONSOLES}"
        for entry in $tmp ; do
            baudrate=`echo $entry | sed 's/\;.*//'`
            ttydev=`echo $entry | sed -e 's/^[0-9]*\;//' -e 's/\;.*//'`
            install -d ${D}${systemd_unitdir}/system/
            install -d ${D}${sysconfdir}/systemd/system/getty.target.wants/
            install -m 0644 ${WORKDIR}/serial-getty@ttyDEV.service \
                ${D}${systemd_unitdir}/system/serial-getty@$ttydev.service
            # Update baudrate and ttydev names
            sed -i -e s/\@BAUDRATE\@/$baudrate/g ${D}${systemd_unitdir}/system/serial-getty@$ttydev.service
            sed -i -e s/\@TERMINAL\@/$ttydev/g ${D}${systemd_unitdir}/system/serial-getty@$ttydev.service

            # enable the service
            ln -sf ${systemd_unitdir}/system/serial-getty@.service \
                ${D}${sysconfdir}/systemd/system/getty.target.wants/serial-getty@$ttydev.service
        done
    fi
}
