##############################################################################
#
# Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################

csr_define_component(CSR_COMPONENT_FRW)
csr_define_component(CSR_COMPONENT_FRW_ACLBUF DEPENDS CSR_COMPONENT_FRW)
csr_define_component(CSR_COMPONENT_FRW_APP DEPENDS CSR_COMPONENT_FRW)
csr_define_component(CSR_COMPONENT_FRW_BSP_APP_BLUECORE DEPENDS CSR_COMPONENT_FRW_BSP_APP_CORE)
csr_define_component(CSR_COMPONENT_FRW_BSP_APP_CORE DEPENDS CSR_COMPONENT_FRW_BSP_CORE)
csr_define_component(CSR_COMPONENT_FRW_BSP_APP_GENERIC DEPENDS CSR_COMPONENT_FRW_BSP_APP_CORE)
csr_define_component(CSR_COMPONENT_FRW_BSP_CORE DEPENDS CSR_COMPONENT_FRW)
csr_define_component(CSR_COMPONENT_FRW_BUILD)
csr_define_component(CSR_COMPONENT_FRW_IP_STACK DEPENDS CSR_COMPONENT_FRW)
csr_define_component(CSR_COMPONENT_FRW_TFTP DEPENDS CSR_COMPONENT_FRW)
csr_define_component(CSR_COMPONENT_FRW_TLS DEPENDS CSR_COMPONENT_FRW)
csr_define_component(CSR_COMPONENT_FRW_TYPE_A DEPENDS CSR_COMPONENT_FRW)
csr_define_component(CSR_COMPONENT_FRW_WIRESHARK DEPENDS CSR_COMPONENT_FRW BINARY)
