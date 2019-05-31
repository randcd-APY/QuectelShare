inherit autotools qcommon qlicense
DESCRIPTION = "vslam"

PR = "r0"
PV = "1.0"

DEPENDS += "glib-2.0 libxml2 libRoboticsCamera libwebp curl qhull libcryptopp"
DEPENDS += "${@base_contains('DISTRO', 'quec-smart-ros',"opencv nav-msgs image-transport kobuki-msgs", '', d)}"
DEPENDS += "${@base_contains('DISTRO', 'quec-smart-ros',"cv-bridge roscpp roslib roscpp-serialization", '', d)}"
DEPENDS += "${@base_contains('DISTRO', 'quec-smart-ros',"rosconsole rostime message-filters xmlrpcpp", '', d)}"
DEPENDS += "${@base_contains('DISTRO', 'quec-smart-ros',"class-loader rosnode rosconsole-bridge rospack", '', d)}"
INSANE_SKIP_${PN} += "dev-deps"

S = "${WORKDIR}/vendor/qcom/proprietary/robot/vslam"
SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/robot/vslam/"

EXTRA_OECONF +=  "${@base_contains('DISTRO', 'quec-smart-ros',"--with-ros", '', d)}"
CXXFLAGS_append = " -I${STAGING_INCDIR}/../../opt/ros/indigo/include/"
CXXFLAGS_append += " -L${STAGING_INCDIR}/../../opt/ros/indigo/lib"

FILES_${PN} += "/usr/bin/*"
FILES_${PN} += "/usr/include/*"
FILES_${PN} += "/usr/lib/*"
FILES_${PN} += "/data/misc/vwslam/Configuration/*"
FILES_${PN} += "/data/vwslam/*"

PACKAGES = "${PN}"

do_install_append() {
   dest=/usr/include/
   install -d ${D}${dest}
   install -m 0644 ${WORKDIR}/vendor/qcom/proprietary/robot/vslam/prebuilt/inc/* -D ${D}${dest}

   dest=/usr/lib/
   install -d ${D}${dest}
   install -m 0644 ${WORKDIR}/vendor/qcom/proprietary/robot/vslam/prebuilt/libs/* -D ${D}${dest}

   dest=/data/misc/vwslam/Configuration/
   install -d ${D}${dest}
   install -m 0644 ${WORKDIR}/vendor/qcom/proprietary/robot/vslam/conf/* -D ${D}${dest}

   dest=/data/vwslam/
   install -d ${D}${dest}
   install -m 0644 ${WORKDIR}/vendor/qcom/proprietary/robot/vslam/conf/readme.txt -D ${D}${dest}
}
