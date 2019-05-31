do_install_append() {
    if ${@bb.utils.contains('DISTRO_FEATURES','selinux','true','false',d)}; then
        # Remove recursive restorecon calls
        sed -i '/^test ! -x \/sbin\/restorecon/ d' ${D}${sysconfdir}/init.d/populate-volatile.sh

        # Replace with a white-list of files that need labeling
        echo "/sbin/restorecon /run/lock /var/volatile/log /var/volatile/tmp /var/lock /var/run /var/tmp /tmp /var/lock/subsys /var/log/wtmp /var/run/utmp /etc/resolv.conf /var/run/resolv.conf /var/log/lastlog || true" >> ${D}${sysconfdir}/init.d/populate-volatile.sh
    fi
}
