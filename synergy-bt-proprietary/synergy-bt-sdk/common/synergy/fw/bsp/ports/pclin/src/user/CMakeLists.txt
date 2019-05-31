###############################################################################
#
# Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################
csr_add_subdirectory(app_main)
csr_add_subdirectory(exceptionhandler)
csr_add_subdirectory(framework_ext)
csr_add_subdirectory(fs)
csr_add_subdirectory(ip GROUP CSR_FRW_GROUP_IPSTACK "Ported")
csr_add_subdirectory(lowlevel/uart)
csr_add_subdirectory(main)
csr_add_subdirectory(panic)
csr_add_subdirectory(pmem)
csr_add_subdirectory(time)
csr_add_subdirectory(tls)
csr_add_subdirectory(util)
csr_add_subdirectory(logtrans)
csr_add_subdirectory(bluecore/transport/hci_socket)
