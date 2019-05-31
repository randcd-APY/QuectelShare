include selinux_20161014.inc
include ${BPN}.inc

LIC_FILES_CHKSUM = "file://COPYING;md5=393a5ca445f6965873eca0259a17f833"

SRC_URI[md5sum] = "0358f0136e2dd9a8c9e99f181aaab1b2"
SRC_URI[sha256sum] = "68891b376f5048edc53c6ccb2fca44da3dc7f4563f4b6894e201d70c04a05a29"

SRC_URI += "\
	file://policycoreutils-fix-sepolicy-install-path.patch \
	file://policycoreutils-make-O_CLOEXEC-optional.patch \
	file://policycoreutils-loadpolicy-symlink.patch \
	file://policycoreutils-process-ValueError-for-sepolicy-seobject.patch \
	file://policycoreutils-fix-TypeError-for-seobject.py.patch \
	file://0001-mcstrans-fix-the-init-script.patch \
	file://enable-mcstrans.patch \
	"
