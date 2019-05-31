SUMMARY = "AllPlay base"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

SRC_URI += " \
	file://300-allplay \
	file://oem.json \
	"

inherit allarch

do_install() {
	install -d ${D}/etc/device_configure.d
	install -m 0755 ${WORKDIR}/300-allplay -D ${D}/etc/device_configure.d/

	install -d ${D}${sysconfdir}/allplay/
	install -m 0644 ${WORKDIR}/oem.json ${D}${sysconfdir}/allplay/

	if [ -d ${THISDIR}/audio_cues ]; then
		install -d ${D}${datadir}/orbPlayer_cues/
		install -m 0644 ${THISDIR}/audio_cues/* ${D}${datadir}/orbPlayer_cues/
	fi
}

FILES_${PN} += " \
	${datadir}/orbPlayer_cues/ \
	${sysconfdir}/allplay/ \
	"
