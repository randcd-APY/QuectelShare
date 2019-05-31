##############################################################################
#
# Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

csr_define_group(CSR_FRW_GROUP_IPSTACK "IP Stack"
                 OPTION "Ported" DEFAULT
                 OPTION "Generic" PLATFORM_DEFAULT BDB3)

csr_define_group(CSR_FRW_GROUP_SDIO "SDIO Stack"
                 OPTION "Ported" DEFAULT
                 OPTION "Generic" PLATFORM_DEFAULT BDB3)

csr_define_group(CSR_FRW_GROUP_APP_BOOTSTRAP "BlueCore bootstrapping method"
                 OPTION "File" DEFAULT
                 OPTION "String"
                 OPTION "Platform")
