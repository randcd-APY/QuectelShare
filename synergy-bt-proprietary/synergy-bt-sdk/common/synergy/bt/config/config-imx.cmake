include(${CSR_BT_ROOT}/config/config-full.cmake)

# Configuration for IMX List Here:
csr_set_cache_parameter(CSR_HOST_PLATFORM VALUE "IMX")

csr_set_cache_parameter(CSR_BT_INSTALL_INTERNAL_APP_DEPENDENCIES VALUE ON)
# Configuration for IMX6:
# 1). When INIT A2DP as SRC, CSR_BT_APP_AV_FILTER VALUE = "sbcwav" in Demo
# 2). When INIT A2DP as SNK, CSR_BT_APP_AV_FILTER VALUE = "sbcaudio" in Demo
csr_set_cache_parameter(CSR_BT_APP_AV_FILTER VALUE "sbc_SRCwave_SNKaudio")
add_definitions("-DAUDIO_FOR_IMX")
