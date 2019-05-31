inherit qcommon qprebuilt qlicense systemd

DESCRIPTION = "Library and routing applications for diagnostic traffic"
DEPENDS = "common glib-2.0 time-genoff liblog"

PR = "r10"

FILESPATH =+ "${WORKSPACE}/:"
SRC_URI   = "file://diag/"
SRC_URI  += "file://chgrp-diag"
SRC_URI  += "file://chgrp-diag.service"

SRC_DIR = "${WORKSPACE}/diag"
S       = "${WORKDIR}/diag"

EXTRA_OECONF += "--with-glib \
                 --with-common-includes=${STAGING_INCDIR} \
                 --enable-target=${BASEMACHINE}"

do_install_append() {
        if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
          install -m 0755 ${WORKDIR}/chgrp-diag -D ${D}${sysconfdir}/initscripts/chgrp-diag
          install -d ${D}/etc/systemd/system/
          install -m 0644 ${WORKDIR}/chgrp-diag.service -D ${D}/etc/systemd/system/chgrp-diag.service
          install -d ${D}/etc/systemd/system/multi-user.target.wants/
          # enable the service for multi-user.target
          ln -sf /etc/systemd/chgrp-diag.service \
             ${D}/etc/systemd/system/multi-user.target.wants/chgrp-diag.service
        else
          install -m 0755 ${WORKDIR}/chgrp-diag -D ${D}${sysconfdir}/init.d/chgrp-diag    
        fi
}

pkg_postinst_${PN} () {

       if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'false', 'true', d)}; then
        [ -n "$D" ] && OPT="-r $D" || OPT="-s"
        update-rc.d $OPT -f start_diag_qshrink4_daemon remove
        update-rc.d $OPT start_diag_qshrink4_daemon start 15 2 3 4 5 . stop 15 0 6 .
        
        update-rc.d $OPT -f chgrp-diag remove
        update-rc.d $OPT chgrp-diag start 15 2 3 4 5 .
       fi
}

FILES_${PN} += "${systemd_unitdir}/system/"

CFLAGS_append = " -DUSE_ANDROID_LOGGING "
LDFLAGS_append = " -llog "
