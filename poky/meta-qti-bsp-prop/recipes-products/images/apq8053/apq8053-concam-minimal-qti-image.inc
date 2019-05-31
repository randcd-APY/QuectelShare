# Packages installed to the root file system

IMAGE_INSTALL += "adsprpc"

IMAGE_INSTALL += "modem-shutdown"
IMAGE_INSTALL += "msmirqbalance"
IMAGE_INSTALL += "start-scripts-misc-daemon"
IMAGE_INSTALL += "start-scripts-qseecomd-daemon"
IMAGE_INSTALL += "start-scripts-sfs-config"

IMAGE_INSTALL += "sensors"
IMAGE_INSTALL += "abctl"

IMAGE_PREPROCESS_COMMAND_prepend = "generate_fsconfig;"

#TODO: Needs clean-up, create separate files that are copied to $WORKDIR instead of current mechanism of string redirection.
generate_fsconfig() {
   # Setup fsconfig for usrfs image
   touch ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc 1000 1000 775" > ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/camera 1006 1000 775" > ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf 5001 1000 775" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/360_cam.conf 5001 5001 644" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/audio_config 5001 5001 644" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/channel1 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/channel2 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/channel3 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/dist 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/dist/index.html 5001 5001 644" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/dist/static 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/dist/static/css 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/dist/static/css/app.css 5001 5001 644" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/dist/static/img 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/dist/static/img/bg.jpg 5001 5001 644" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/dist/static/js 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/dist/static/js/app.js 5001 5001 644" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/dist/static/js/manifest.js 5001 5001 644" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/dist/static/js/vendor.js 5001 5001 644" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/image 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/ip_cam.conf 5001 5001 644" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/net_config 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/res_config 5001 5001 644" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/vam 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/qmmf/ipc_webserver/video 5001 5001 755" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "misc/vam 5001 1000 775" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "persist 5002 1000 775" >> ${WORKDIR}/usrfs_fsconfig.conf
   echo "persist/build.prop 5002 5002 664" >> ${WORKDIR}/usrfs_fsconfig.conf

   # Setup fsconfig for persist image
   touch ${WORKDIR}/persist_fsconfig.conf
   echo "sensors 3011 1000 775" > ${WORKDIR}/persist_fsconfig.conf
   echo "sensors/sensors_settings 3011 1000 660" >> ${WORKDIR}/persist_fsconfig.conf
   echo "sensors/sns.reg 3011 1000 660" >> ${WORKDIR}/persist_fsconfig.conf
}
