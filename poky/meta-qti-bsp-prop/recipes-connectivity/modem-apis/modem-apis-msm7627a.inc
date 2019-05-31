SRC_URI = "file://${WORKSPACE}/modem-apis/msm7627a"
S = "${WORKDIR}/msm7627a"

defines_api_features = -DAUTH_A_KEY_DIGITS=26
defines_api_features += -DREX_TASK_NAME_LEN=127
# cm_api_specific_defines
defines_api_features += -DFEATURE_MMGPS
defines_api_features += -DFEATURE_PDAPI
defines_api_features += -DFEATURE_CCBS
defines_api_features += -DFEATURE_REL5
defines_api_features += -DFEATURE_TTY
defines_api_features += -DFEATURE_GSM_EXT_SPEECH_PREF_LIST
defines_api_features += -DFEATURE_UUS
defines_api_features += -DFEATURE_MM_SUPERSET
defines_api_features += -DFEATURE_GSM_AMR_WB
# cm_target_specific
defines_api_features += -DFEATURE_GSM_AMR
defines_api_features += -DFEATURE_GSM_HALF_RATE
# end_cm_target_specific

# dsucsd_api_feature_defines
defines_api_features += -DFEATURE_DATA
defines_api_features += -DFEATURE_DATA_GCSD
defines_api_features += -DFEATURE_DATA_WCDMA_CS
defines_api_features += -DFEATURE_DATA_UCSD_API_SIOPORT
defines_api_features += -DFEATURE_DATA_UCSD_SCUDIF_API
defines_api_features += -DFEATURE_DATA_UCSD_UNIFIED_API
defines_api_features += -DFEATURE_CM_MMGPS
defines_api_features += -DFEATURE_USB_CDC_ACM
defines_api_features += -DFEATURE_SMD
defines_api_features += -DFEATURE_SMEM_DS
# end_dsucsd

# gsdi_api_specific_defines
defines_api_features += -DFEATURE_MMGSDI_UMTS
defines_api_features += -DFEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC
defines_api_features += -DASSERT
# end_gsdi

# mvs api specific defines
defines_api_features += -DFEATURE_MVS
# end_mvs

# NV defines (ported from the NV Makefile)
defines_api_features += -DFEATURE_NV_RPC_SUPPORT

# pdapi, pdsm_atl specific defines
defines_api_features += -DFEATURE_PDAPI
# end_pdapi, pdsm_atl

# pmapp_gen api specific defines
defines_api_features += -DFEATURE_PMIC_RTC
defines_api_features += -DFEATURE_EXTERNAL_ERR_FATAL
# end_pmapp_gen

# pmapp_otg api specific defines
defines_api_features += -DFEATURE_PMIC_USB_OTG
# end_pmapp_otg

# snd api specific defines
defines_api_features += -DFEATURE_VOC_PCM_INTERFACE
defines_api_features += -DFEATURE_VOICE_PLAYBACK
defines_api_features += -DFEATURE_VOICE_RECORD
# end_snd

# wms api specific defines
defines_api_features += -DFEATURE_GWSMS
defines_api_features += -DFEATURE_SMS_UDH
defines_api_features += -DFEATURE_BROADCAST_SMS_MULTIMODE
# end_wms

defines_api_features += -DFEATURE_ADIE_SVC

# begin add_clkregim
defines_api_features += -DT_CLKREGIM=7625

defines_api_features += -DFEATURE_CLKREGIM_RM
# end_add_clkregim

defines_api_features += -DFEATURE_GWSMS

defines_api_features += -DFEATURE_WCDMA
defines_api_features += -DFEATURE_GSM
