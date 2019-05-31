inherit autotools

DESCRIPTION = "Installing alsaucm configuration files"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD;md5=3775480a712fc46a69647678acb234cb"
PR = "r0"

SRC_URI = "file://snd_soc_msm_I2S/* \
          "
S = "${WORKDIR}"

FILES_${PN} = "${datadir}/alsa/ucm/"

do_install() {
      install -d ${D}${datadir}/alsa/ucm/conga/
      install -m 0755 ${S}/snd_soc_msm_I2S/conga/* -D ${D}${datadir}/alsa/ucm/conga/
      install -d ${D}${datadir}/alsa/ucm/wcd9326/
      install -m 0755 ${S}/snd_soc_msm_I2S/wcd9326/* -D ${D}${datadir}/alsa/ucm/wcd9326/
}

