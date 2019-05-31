##############################################################################
#
# Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
##############################################################################

# Add the technologies to the build system.

if (CSR_PLATFORM STREQUAL BDB3)
    if (NOT CSR_NUCLEUS_ROOT OR "${CSR_NUCLEUS_ROOT}" STREQUAL "")
        set(CSR_NUCLEUS_ROOT "${CSR_INSTALL_ROOT}/nucleus/2.6.2")
    endif()

    # Backward compatibility with Nucleus 2.6.2 which tries to include
    # ${CSR_BUILD_ROOT}/{CSR_BUILD_RULES} which does not exist anymore.
    set(CSR_BUILD_RULES Compiler/ARM-ASM.cmake)

    csr_add_technology(NAME NUCLEUS
                       DISPLAY_NAME "Nucleus"
                       DESCRIPTION "Nucleus")
endif()

csr_add_technology(NAME FRW
                   DISPLAY_NAME "Framework"
                   DESCRIPTION "Synergy Framework")

csr_add_technology(NAME BT
                   DISPLAY_NAME "Bluetooth"
                   DESCRIPTION "Synergy Bluetooth")

csr_add_technology(NAME WIFI
                   DISPLAY_NAME "Wi-Fi"
                   DESCRIPTION "Synergy Wi-Fi")

csr_add_technology(NAME MERCURY
                   DISPLAY_NAME "Mercury"
                   DESCRIPTION "Synergy Mercury")
