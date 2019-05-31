##############################################################################
#
# Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(FILES
    csr_synergy.cmake
    csr_synergy.h.in
    technologies.cmake)

csr_add_files(COMPONENT CSR_COMPONENT_FRW_BUILD FILES ${FILES})
