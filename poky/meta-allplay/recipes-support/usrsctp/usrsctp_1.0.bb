LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=ffcf846341f3856d79a483eafa18e2a5"

SRC_URI = "git://github.com/sctplab/usrsctp;protocol=https"
SRC_URI += " \
            file://001-check-sb_state-for-userspace_linux.patch \
            file://002-wakeup-accept-on-shutdown.patch \
            file://003-provide-getter-for-udp-socket.patch \
            file://004-add-TTL-support-on-send.patch \
            file://005-refresh-net-interfaces.patch \
            file://006-wakeup-connect-on-shutdown.patch \
            file://007-add-recv-timeout.patch \
            file://008-add-connect-timeout.patch \
            file://009-fix_le_compilation_issues.patch \
            file://010-clear-asoc-read-q-on-shutdown.patch \
            file://011-exclude-bridge0-local-interface.patch \
            "

SRCREV = "e95114a82787f5d58269c127de085c24b5e00243"

S = "${WORKDIR}/git"

inherit autotools

