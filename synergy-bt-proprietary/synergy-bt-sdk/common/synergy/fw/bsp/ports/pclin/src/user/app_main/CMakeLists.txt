##############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################
csr_include_directories(COMPONENT CSR_COMPONENT_FRW_BSP_APP_BLUECORE 
../../../../../../../../../synergy-middleware/common/inc)

set(SRCS_COMMON
    csr_app_main.c
    csr_key_input.c
    csr_ui_lower.c)

set(SRCS_BLUECORE
    csr_app_main_transport_bluecore.c)

set(SRCS_PLAIN
    csr_app_main_transport.c)

set(HDRS
    csr_app_main_transport.h
    csr_key_input.h
    csr_phone_skin.h
    csr_ui_lower_initialise.h)

set(PUBLIC_HDRS
    ../../../../../inc/csr_app_main.h)

csr_add_library(csr_app_main_bluecore
                COMPONENT CSR_COMPONENT_FRW_BSP_APP_BLUECORE
                SOURCES ${SRCS_COMMON} ${SRCS_BLUECORE}
                HEADERS ${HDRS}
                PUBLIC_HEADERS ${PUBLIC_HDRS}
                ALLOW_NON_ANSI)

csr_add_library(csr_app_main
                COMPONENT CSR_COMPONENT_FRW_BSP_APP_GENERIC
                SOURCES ${SRCS_COMMON} ${SRCS_PLAIN}
                HEADERS ${HDRS}
                PUBLIC_HEADERS ${PUBLIC_HDRS}
                ALLOW_NON_ANSI)

# [QTI] NOT add "BSP_SUPPORT_SDL", due to only console supported.
#if (SDL_FOUND)
#    add_definitions(-DBSP_SUPPORT_SDL)
#endif()
