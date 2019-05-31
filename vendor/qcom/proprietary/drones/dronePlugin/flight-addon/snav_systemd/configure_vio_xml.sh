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

usage()
{
  echo -e "usage:\n"
  echo -e "-c [downward_45_tilt|downward]"
  echo -e "       Select camera (for vio only); default is downward_45_tilt."
  echo -e "       Downward used with vio is experimental only and not"
  echo -e "       recommended."
  echo -e "-h     Print this message."
}

USER=$(whoami)

if [ "${USER}" != "root" ]; then
  echo "Run this script as the root user"
  exit 1
fi

# Parse arguments
while getopts "c:h" OPTION
do
  case $OPTION in
    c) CAMERA=$OPTARG
       ;;
    h) usage
       exit 1
       ;;
    *)
      usage
      exit 1
      ;;
  esac
done
shift $(($OPTIND - 1))

echo "Configuring for vio"
if [ -z "${CAMERA}" ]; then
  echo "No camera specified. Defaulting to downward_45_tilt"
  CAMERA="downward_45_tilt"
elif [ "${CAMERA}" != "downward" ] && [ "${CAMERA}" != "downward_45_tilt" ]; then
  echo "Invalid camera specified."
  usage
  exit 1
fi

if [ "${CAMERA}" == "downward" ]; then
  echo "Will use downward camera"
  echo "WARNING! downward is experimental"
  echo "downward_45_tilt is strongly recommended for best performance"
  ln -sf /etc/snav/mount.downward.xml /etc/snav/mount.snav_dft_vio_app.xml
elif [ "${CAMERA}" == "downward_45_tilt" ]; then
  echo "Will use downward camera mounted with 45 deg tilt"
  ln -sf /etc/snav/mount.downward_45_tilt.excelsior.xml /etc/snav/mount.snav_dft_vio_app.xml
fi

exit 0

