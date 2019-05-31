###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

set(SRCS
    bt_test_main.c
    ../../bt_bootstrap/qca/bt_h4_transport.c
    ../../bt_bootstrap/util/connx_serial_com.c
    ../../bt_bootstrap/util/bt_bootstrap_util.c
    ../../bt_bootstrap/util/connx_log_btsnoop.c
    ../../util/connx_util.c
    ../../util/connx_log.c
    ../../util/connx_time.c
    ../../util/connx_arg_search.c
    ../../util/connx_log_setting.c
    ../../util/connx_setting.c
    ../../util/connx_file.c
    ../../util/connx_list.c)

set(HDRS)

add_executable(bt_test ${SRCS} ${HDRS})
target_link_libraries(bt_test ${LIBS} ${CONNX_SYSTEM_LIBS})
