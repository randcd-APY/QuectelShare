##############################################################################
#
# Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

set(SRCS)

set(HDRS)

set(PUBLIC_HDRS
    csr_types.h)

csr_add_headers(COMPONENT CSR_COMPONENT_FRW ${PUBLIC_HDRS} PLATFORM LINUX ANDROID)
