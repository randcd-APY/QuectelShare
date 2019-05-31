require conf/distro/qpermissions.conf

QPERM_SERVICE ?= ""

do_update_service () {
    set +e
    export SERVICES="${QPERM_SERVICE}"
    if [ "${SERVICES}" != "" ] ; then
        if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
            for service in ${SERVICES}; do
                sed -i "/User=/d;/Group=/d;/CapabilityBoundingSet=/d" ${service}
            done
        fi
    fi
}


do_update_files() {
    set +e
    export FILE_PERMISSIONS="${QPERM_FILE}"
    if [ "$FILE_PERMISSIONS" != "" ] ; then
        for each_file in ${FILE_PERMISSIONS};    do
            path="$(cut -d ":" -f 1 <<< $each_file)"
            user="$(cut -d ":" -f 2 <<< $each_file)"
            group="$(cut -d ":" -f 3 <<< $each_file)"
            chown $user:$group ${D}$path
        done
    fi
}

do_install[postfuncs] += "${@['','do_update_files'][(d.getVar('QPERMISSIONS_ENABLE', True) == '1')]}"

python __anonymous() {
# If QPERMISSIONS are not Enabled, add update service premissions task
    if (d.getVar('QPERMISSIONS_ENABLE', True) == '0'):
        bb.build.addtask('do_update_service', 'do_install', 'do_compile', d)
}
