#!/bin/bash

#/*****************************************************************************
# *
# * Copyright 2015-2017 Qualcomm Technologies, Inc.  All rights reserved.
# *
# * This software may be subject to U.S. and international export, re-export
# * or transfer laws. Diversion contrary to U.S. and international law is
# * strictly prohibited.
# *
# * The party receiving this software directly from QTI (the "Recipient")
# * may use this software solely as set forth in the agreement between the
# * Recipient and QTI (the "Agreement"). The software may be used in source
# * code form solely by the Recipient's employees (if any) authorized by the
# * Agreement. Unless expressly authorized in the Agreement, the Recipient
# * may not sublicense, assign, transfer or otherwise provide the source
# * code to any third party. Qualcomm Technologies, Inc. retains all
# * ownership rights in and to the software. Except as may be expressly
# * granted by the Agreement, this file provides no license to any patents,
# * trademarks, copyrights, or other intellectual property of QUALCOMM
# * Incorporated or its affiliates.
# *
# * This notice supersedes any other QTI notices contained within the
# * software except copyright notices indicating different years of
# * publication for different portions of the software. This notice does not
# * supersede the application of any third party copyright notice to that
# * third party's code.
# ****************************************************************************/

set -e

if [ "$1" = "vio" ] ; then

    adb shell "mv /usr/bin/start /usr/bin/start_disabled"     > /dev/null
    adb shell "mv /usr/bin/stop /usr/bin/stop_disabled"       > /dev/null
    adb shell "mv /usr/bin/restart /usr/bin/restart_disabled" > /dev/null

    adb push snav.service /etc/systemd/system/.
    adb push snav_dronecontroller.service /etc/systemd/system/.
    adb push snav_vio_app.service /etc/systemd/system/.
    adb push configure_vio_xml.sh /home/root/.

    adb shell "setenforce 0"
    adb shell "sed -i 's/SELINUX=enforcing/SELINUX=permissive/' /etc/selinux/config"
    adb shell "systemctl daemon-reload"
    adb shell "systemctl enable snav.service"
    adb shell "systemctl enable snav_dronecontroller.service"
    adb shell "systemctl enable snav_vio_app.service"
    adb shell "/home/root/configure_vio_xml.sh"

    echo "Rebooting ..."
    adb reboot
    adb wait-for-devices

elif [ "$1" = "voa" ] ; then

    adb shell "mv /usr/bin/start /usr/bin/start_disabled"     > /dev/null
    adb shell "mv /usr/bin/stop /usr/bin/stop_disabled"       > /dev/null
    adb shell "mv /usr/bin/restart /usr/bin/restart_disabled" > /dev/null

    adb push snav.service /etc/systemd/system/.
    adb push snav_dronecontroller.service /etc/systemd/system/.
    adb push snav_vio_app.service /etc/systemd/system/.
    adb push snav_voa_app.service /etc/systemd/system/.
    adb push snav_stereo_setup.service /etc/systemd/system/.
    adb push configure_voa_xml.sh /home/root/.

    adb shell "setenforce 0"
    adb shell "sed -i 's/SELINUX=enforcing/SELINUX=permissive/' /etc/selinux/config"
    adb shell "systemctl daemon-reload"
    adb shell "systemctl enable snav.service"
    adb shell "systemctl enable snav_dronecontroller.service"
    adb shell "systemctl enable snav_vio_app.service"
    adb shell "systemctl enable snav_voa_app.service"
    adb shell "systemctl enable snav_stereo_setup.service"
    adb shell "chmod +x /home/root/configure_voa_xml.sh"
    adb shell "/home/root/configure_voa_xml.sh -a 0"

    echo "Rebooting ..."
    adb reboot
    adb wait-for-devices

else

  echo "Specify install type: vio or voa."

fi

