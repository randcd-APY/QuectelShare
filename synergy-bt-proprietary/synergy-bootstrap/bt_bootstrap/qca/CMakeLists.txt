###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

set(SRCS
    boot_qca.c 
    bt_h4_transport.c 
    qca_fw_config.c 
    qca_h4_transport.c)

set(HDRS)

add_library(boot_qca ${SRCS} ${HDRS})