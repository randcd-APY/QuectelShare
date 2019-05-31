###############################################################################
#
# Copyright (c) 2012-2017 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################

set(SRCS
    csr_bt_av2_main.c
    csr_bt_av2_utils.c
    csr_bt_av2_avup.c
    csr_bt_av2_avhandlers.c
    csr_bt_av2_avdown.c
    csr_bt_av2_filter.c
    csr_bt_filter_av2.c
    csr_bt_av_app_ui_sef.c
    csr_bt_av_app_util.c
    csr_bt_av_app_lib.c
    csr_bt_av_app_sef.c
    csr_bt_filter_filedump.c)

set(HDRS
    csr_bt_aac_parser.h
    csr_bt_av2.h
    csr_bt_av2_filter.h
    csr_bt_av_app_ui_sef.h
    csr_bt_av_app_ui_strings.h
    csr_bt_av_app_util.h
    csr_bt_av_app_sef.h
    ../inc/csr_bt_av_app_prim.h
    ../inc/csr_bt_av_app_lib.h
    ../inc/csr_bt_av_app_task.h )


csr_install_files(COMPONENT CSR_COMPONENT_BT_APP_PORTING csr_bt_filter_sbc_pts.c)

set(SRCS_AAC
        csr_bt_aac_parser.c
        csr_bt_filter_aac.c)

set(SRCS_MP3
    csr_bt_filter_mp3.c)

set(SRCS_SBCWAV
        csr_bt_filter_sbc.c
        csr_bt_filter_wave.c)

set(SRCS_SBCAUDIO
        csr_bt_filter_sbc.c)

set(SRCS_SBCFILE
        csr_bt_filter_sbcfile.c)

set(SRCS_AVROUTER
        csr_bt_filter_avrouter.c)

set(SRCS_WIN
        csr_bt_filter_winplay.c 
        csr_bt_filter_wingrab.c)

set(SRCS_LINUX
        csr_bt_filter_alsa.c )

set(SRCS_SBC_SRC_WAV_SNK_AUDIO
        csr_bt_filter_sbc.c
        csr_bt_filter_wave.c
        csr_bt_filter_alsa.c)
                        
csr_add_library(csr_bt_app_av
                COMPONENT CSR_COMPONENT_BT_APP_LIB
                SOURCES ${SRCS}
                CONDITIONAL CSR_BT_APP_AV_FILTER STREQUAL "mp3" SOURCES ${SRCS_MP3}
                CONDITIONAL CSR_BT_APP_AV_FILTER STREQUAL "aac" SOURCES ${SRCS_AAC}
                CONDITIONAL CSR_BT_APP_AV_FILTER STREQUAL "sbcwav" SOURCES ${SRCS_SBCWAV}
                CONDITIONAL CSR_BT_APP_AV_FILTER STREQUAL "sbcaudio" SOURCES ${SRCS_SBCAUDIO}
                CONDITIONAL CSR_BT_APP_AV_FILTER STREQUAL "sbcfile" SOURCES ${SRCS_SBCFILE}
                CONDITIONAL CSR_BT_APP_AV_FILTER STREQUAL "avrouter" SOURCES ${SRCS_AVROUTER}                
                CONDITIONAL CSR_PLATFORM MATCHES WINDOWS AND CSR_BT_APP_AV_FILTER STREQUAL "sbcaudio" SOURCES ${SRCS_WIN}
                CONDITIONAL CSR_PLATFORM MATCHES LINUX AND CSR_BT_APP_AV_FILTER STREQUAL "sbcaudio" SOURCES ${SRCS_LINUX}
                CONDITIONAL CSR_PLATFORM MATCHES LINUX AND CSR_BT_APP_AV_FILTER STREQUAL "sbc_SRCwave_SNKaudio" SOURCES ${SRCS_SBC_SRC_WAV_SNK_AUDIO}
                HEADERS ${HDRS}
                LIBRARIES ${LIBS} SYSTEM
                ALLOW_WARNINGS)


if(NOT EXCLUDE_CSR_BT_ASM_MODULE AND NOT CSR_BT_INTEROP_TEST_BUILD)
    set_property(TARGET csr_bt_app_av APPEND PROPERTY COMPILE_DEFINITIONS USE_AVROUTER)
else()
    if((CSR_BT_APP_AV_FILTER STREQUAL "aac") AND (TARGET csr_bt_app_av))
        set_property(TARGET csr_bt_app_av APPEND PROPERTY COMPILE_DEFINITIONS "USE_AAC;USE_FILEDUMP")
    endif()
    
    
    if((CSR_BT_APP_AV_FILTER STREQUAL "mp3") AND (TARGET csr_bt_app_av))
        set_property(TARGET csr_bt_app_av APPEND PROPERTY COMPILE_DEFINITIONS "USE_MP3;USE_FILEDUMP")
    endif()
    
    if((CSR_BT_APP_AV_FILTER STREQUAL "sbcwav")  AND (TARGET csr_bt_app_av))
        set_property(TARGET csr_bt_app_av APPEND PROPERTY COMPILE_DEFINITIONS "USE_SBC;USE_WAVE")
    endif()
    
    # Autodetect audio system
    if ((CSR_PLATFORM MATCHES WINDOWS) AND (CSR_BT_APP_AV_FILTER STREQUAL "sbcaudio")  AND (TARGET csr_bt_app_av))
        set_property(TARGET csr_bt_app_av APPEND PROPERTY COMPILE_DEFINITIONS USE_WINAUDIO)
    endif()
    
    if ((CSR_PLATFORM MATCHES LINUX AND CSR_BT_APP_AV_FILTER STREQUAL "sbcaudio") AND (TARGET csr_bt_app_av))
        set_property(TARGET csr_bt_app_av APPEND PROPERTY COMPILE_DEFINITIONS "USE_ALSA `pkg-config --cflags alsa`")
    endif()
    
    if ((CSR_PLATFORM MATCHES LINUX AND CSR_BT_APP_AV_FILTER STREQUAL "sbc_SRCwave_SNKaudio") AND (TARGET csr_bt_app_av))
        set_property(TARGET csr_bt_app_av APPEND PROPERTY COMPILE_DEFINITIONS "USE_SBC; USE_WAVE; USE_ALSA; USE_WAVE_SRC_ALSA_SNK_ONLY `pkg-config --cflags alsa`")
    endif()
 
    if((CSR_BT_APP_AV_FILTER STREQUAL "sbcaudio") AND (TARGET csr_bt_app_av))
        set_property(TARGET csr_bt_app_av APPEND PROPERTY COMPILE_DEFINITIONS USE_SBC)
    endif()
    
    if((CSR_BT_APP_AV_FILTER STREQUAL "sbcfile") AND (TARGET csr_bt_app_av))
        set_property(TARGET csr_bt_app_av APPEND PROPERTY COMPILE_DEFINITIONS USE_SBCFILE)
    endif()
endif()
