###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

set(SRCS
    btdiag.c 
    qcmbr.c 
    ../../bt_bootstrap/qca/qca_h4_transport.c 
    ../../bt_bootstrap/qca/bt_h4_transport.c 
    ../../bt_bootstrap/util/connx_serial_com.c 
    ../../bt_bootstrap/util/bt_bootstrap_util.c 
    ../../bt_bootstrap/util/connx_log_btsnoop.c 
    ../../util/connx_util.c 
    ../../util/connx_log.c
    ../../util/connx_file.c
    ../../util/connx_time.c)

set(HDRS
    qcmbr.h 
    global.h)

add_executable(btdiag ${SRCS} ${HDRS})
target_link_libraries(btdiag ${LIBS} ${CONNX_SYSTEM_LIBS})