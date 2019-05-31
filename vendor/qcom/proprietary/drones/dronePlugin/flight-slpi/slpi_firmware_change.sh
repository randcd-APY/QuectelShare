#!/bin/sh
###############################################################################
#
# This script is used for System V init scripts to update slpi firmware images
#
# Copyright (c) 2012-2016 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
###############################################################################
ORIG_IMG_PATH="/lib/firmware/drones_image"
DEST_IMG_PATH="/firmware/image"

cd $ORIG_IMG_PATH
for img_file in `find ./ -name "slpi*.*" `
do
    if [ ! -s ${DEST_IMG_PATH}/${img_file} ]; then
        cp ${img_file} ${DEST_IMG_PATH}
    fi

    while [ true ]; do
        orig_sum=`md5sum ${img_file} |awk '{print $1}' `
        dest_sum=`md5sum ${DEST_IMG_PATH}/${img_file} |awk '{print $1}' `

        if [ "${orig_sum}" != "${dest_sum}" ]; then
            cp ${img_file} ${DEST_IMG_PATH}
            sync
        else
            break
        fi
    done
done
cd -
