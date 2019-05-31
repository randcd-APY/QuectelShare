DESCRIPTION = "QTI Alexa Voice Client"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://COPYING;md5=c5e7ec9b8d9955820c3d2536d9e434cd"

FILESPATH_prepend = "${WORKSPACE}:"

SRC_URI = "file://vendor/qcom/proprietary/alexa-voice-client/ \
           file://alexaclient.service \
           file://alexa.uim \
           file://med_system_alerts_melodic_01._TTH_.mp3 \
           file://med_system_alerts_melodic_02._TTH_.mp3 \
           file://med_system_alerts_melodic_01_short._TTH_.wav \
           file://med_system_alerts_melodic_02_short._TTH_.wav "
S = "${WORKDIR}/vendor/qcom/proprietary/alexa-voice-client/"
SRCREV = "${AUTOREV}"

inherit cmake update-rc.d systemd

DEPENDS += "gcc (>= 4.8) cmake (>= 3.0) openssl (>= 1.0.2) curl (>= 7.44.0)"
DEPENDS += "libinput libevdev libinputhelper avs-device-sdk gst-plugins"
EXTRA_OECMAKE_append_msm8909 += "-DHWAPQ8009=ON" 
# Specify any options you want to pass to cmake using EXTRA_OECMAKE:
EXTRA_OECMAKE = "-DGSTREAMER_MEDIA_PLAYER=ON \
                 -DSYSROOT_DIR=${PKG_CONFIG_SYSROOT_DIR}"

INITSCRIPT_NAME = "alexaclientd"
INITSCRIPT_PARAMS = "start 99 2 3 4 5 ."

python do_getpatches() {
    import os

    cmd = "cd ${WORKSPACE}/poky/meta-qti-avs-prop/recipes/alexa-voice-client/files \
    && (wget -c https://images-na.ssl-images-amazon.com/images/G/01/mobile-apps/dex/alexa/alexa-voice-service/docs/audio/states/med_system_alerts_melodic_02._TTH_.mp3 || pwd) \
    && (wget -c https://images-na.ssl-images-amazon.com/images/G/01/mobile-apps/dex/alexa/alexa-voice-service/docs/audio/states/med_system_alerts_melodic_02_short._TTH_.wav || pwd) \
    && (wget -c https://images-na.ssl-images-amazon.com/images/G/01/mobile-apps/dex/alexa/alexa-voice-service/docs/audio/states/med_system_alerts_melodic_01._TTH_.mp3 || pwd) \
    && (wget -c https://images-na.ssl-images-amazon.com/images/G/01/mobile-apps/dex/alexa/alexa-voice-service/docs/audio/states/med_system_alerts_melodic_01_short._TTH_.wav || pwd)"

    os.system(cmd)
}

addtask getpatches before do_fetch

do_install_append() {
  install -d ${D}${bindir}
  install -m 755 ${D}/../build/Application/app/AlexaClientApp ${D}${bindir}

  if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
       install -b -m 0644 /dev/null -D ${D}${sysconfdir}/alexa/AlexaClientSDKConfig.json
       install -d ${D}${systemd_unitdir}/system/
       install -m 0644 ${WORKDIR}/alexaclient.service -D ${D}${systemd_unitdir}/system/alexaclient.service
       install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
       # enable the service for multi-user.target
       ln -sf ${systemd_unitdir}/system/alexaclient.service \
            ${D}${systemd_unitdir}/system/multi-user.target.wants/alexaclient.service
  else
       install -m 0755 ${S}/Application/start_alexaclientd -D ${D}${sysconfdir}/init.d/alexaclientd
  fi

  install -d ${D}${sysconfdir}/alexa/
  install -m 0644 ${WORKDIR}/alexa.uim ${D}${sysconfdir}/alexa
  install -m 0644 ${WORKDIR}/med_system_alerts_melodic_01._TTH_.mp3 ${D}${sysconfdir}/alexa
  install -m 0644 ${WORKDIR}/med_system_alerts_melodic_02._TTH_.mp3 ${D}${sysconfdir}/alexa
  install -m 0644 ${WORKDIR}/med_system_alerts_melodic_01_short._TTH_.wav ${D}${sysconfdir}/alexa
  install -m 0644 ${WORKDIR}/med_system_alerts_melodic_02_short._TTH_.wav ${D}${sysconfdir}/alexa

  chrpath -d ${D}${bindir}/AlexaClientApp
}

FILES_${PN} += "${libdir}/lib*.so \
              ${bindir}/"
FILES_${PN}-dbg  += "${libdir}/.debug/lib*.so \
                   ${bindir}/.debug/*"

FILES_${PN} += "${systemd_unitdir}/system/"
FILES_${PN} += "${sysconfdir}/alexa/AlexaClientSDKConfig.json \
                ${sysconfdir}/alexa/alexa.uim \
                ${sysconfdir}/alexa/med_system_alerts_melodic_01._TTH_.mp3 \
                ${sysconfdir}/alexa/med_system_alerts_melodic_02._TTH_.mp3 \
                ${sysconfdir}/alexa/med_system_alerts_melodic_01_short._TTH_.wav \
                ${sysconfdir}/alexa/med_system_alerts_melodic_02_short._TTH_.wav "

FILES_SOLIBSDEV = ""
