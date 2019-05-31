ifeq ($(BUILD_ATH6KL_VER_35), 1)
obj-m += wlan.o

wlan-y += drivers/net/wireless/ath/ath6kl-3.5/debug.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/htc_pipe.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/bmi.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/cfg80211.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/cfg80211_btcoex.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/init.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/main.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/txrx.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/wmi.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/wmi_btcoex.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/usb.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/testmode.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/rttm.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/diag.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/htcoex.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/ath_netlink.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/pm.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/p2p.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/ap.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/reg.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/regdb.o
wlan-y += drivers/net/wireless/ath/ath6kl-3.5/msm.o

ccflags-y += -DCONFIG_ATH6KL_UB134 -DUSB_AUTO_SUSPEND

ifeq ($(HAVE_BUS_VOTE), 1)
ccflags-y += -DATH6KL_BUS_VOTE
endif

endif

ifeq ($(BUILD_ATH6KL_VER_32), 1)
obj-m += wlan.o

wlan-y += drivers/net/wireless/ath/ath6kl/debug.o
wlan-y += drivers/net/wireless/ath/ath6kl/debugfs_pri.o
wlan-y += drivers/net/wireless/ath/ath6kl/hif.o
wlan-y += drivers/net/wireless/ath/ath6kl/htc.o
wlan-y += drivers/net/wireless/ath/ath6kl/bmi.o
wlan-y += drivers/net/wireless/ath/ath6kl/cfg80211.o
wlan-y += drivers/net/wireless/ath/ath6kl/cfg80211_btcoex.o
wlan-y += drivers/net/wireless/ath/ath6kl/pm.o
wlan-y += drivers/net/wireless/ath/ath6kl/init.o
wlan-y += drivers/net/wireless/ath/ath6kl/main.o
wlan-y += drivers/net/wireless/ath/ath6kl/txrx.o
wlan-y += drivers/net/wireless/ath/ath6kl/wmi.o
wlan-y += drivers/net/wireless/ath/ath6kl/wmi_btcoex.o
wlan-y += drivers/net/wireless/ath/ath6kl/sdio.o
wlan-y += drivers/net/wireless/ath/ath6kl/platform.o
wlan-y += drivers/net/wireless/ath/ath6kl/softmac.o

wlan-y +=  drivers/net/wireless/ath/ath6kl/testmode.o
wlan-y += drivers/net/wireless/ath/ath6kl/wmiconfig.o
wlan-y += drivers/net/wireless/ath/ath6kl/recovery.o
endif

ifeq ($(HAVE_CFG80211), 1)

obj-$(CONFIG_CFG80211) += cfg80211.o

cfg80211-y += net/wireless/core.o net/wireless/sysfs.o net/wireless/radiotap.o net/wireless/util.o net/wireless/reg.o net/wireless/scan.o net/wireless/nl80211.o
cfg80211-y += net/wireless/mlme.o net/wireless/ibss.o net/wireless/sme.o net/wireless/chan.o net/wireless/ethtool.o net/wireless/mesh.o
cfg80211-$(CONFIG_CFG80211_DEBUGFS) += net/wireless/debugfs.o
cfg80211-y += net/wireless/wext-compat.o net/wireless/wext-sme.o
cfg80211-y += net/wireless/regdb.o

$(obj)/net/wireless/regdb.c:  $(PWD)/$(src)/net/wireless/db.txt $(PWD)/$(src)/net/wireless/genregdb.awk
	@$(AWK) -f $(PWD)/$(src)/net/wireless/genregdb.awk < $< > $@

clean-files := net/wireless/regdb.c

ccflags-y += -DCONFIG_ATH6KL_DEBUG
ccflags-y += -DCONFIG_NL80211_TESTMODE
ccflags-y += -DCONFIG_CFG80211_DEFAULT_PS
ccflags-y += -DCONFIG_CFG80211_WEXT
ccflags-y += -DCONFIG_ATH6KL_REGDOMAIN
ccflags-y += -DCONFIG_CFG80211_INTERNAL_REGDB
ccflags-y += -D__CHECK_ENDIAN__

ccflags-y += -I../external/compat-wireless/include
ccflags-y += -I../external/compat-wireless/net/wireless
ccflags-y += -include include/linux/ieee80211.h
ccflags-y += -include include/linux/nl80211.h
ccflags-y += -include include/net/cfg80211.h
ccflags-y += -include include/linux/compat-2.6.h
ccflags-y += -include ../../$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/linux/version.h
else
ifeq ($(HAVE_CFG80211_KERNEL3_4), 1)
ccflags-y += -DATH6KL_SUPPORT_NL80211_KERNEL3_4
endif
ifeq ($(HAVE_CFG80211_KERNEL3_7), 1)
ccflags-y += -DATH6KL_SUPPORT_NL80211_KERNEL3_6
ccflags-y += -DATH6KL_SUPPORT_NETLINK_KERNEL3_6
ccflags-y += -DATH6KL_SUPPORT_NETLINK_KERNEL3_7
endif
ccflags-y += -DCONFIG_ATH6KL_DEBUG
ccflags-y += -DCONFIG_ATH6KL_REGDOMAIN
ccflags-y += -D__CHECK_ENDIAN__
endif

ifeq ($(BUILD_ATH_ETH_ALX), 1)

obj-m += eth.o
eth-y += drivers/net/ethernet/atheros/alx/alx_main.o
eth-y += drivers/net/ethernet/atheros/alx/alx_ethtool.o
eth-y += drivers/net/ethernet/atheros/alx/alc_cb.o
eth-y += drivers/net/ethernet/atheros/alx/alc_hw.o
eth-y += drivers/net/ethernet/atheros/alx/alf_cb.o
eth-y += drivers/net/ethernet/atheros/alx/alf_hw.o

obj-m += compat.o
compat-y += compat/main.o
compat-y += compat/compat-3.2.o

ccflags-y += -D__CHECK_ENDIAN__

EXTRA_CFLAGS += \
        -DCOMPAT_BASE_TREE="\"$(shell cat $(PWD)/$(src)/compat_base_tree)\"" \
        -DCOMPAT_BASE_TREE_VERSION="\"$(shell cat $(PWD)/$(src)/compat_base_tree_version)\"" \
        -DCOMPAT_PROJECT="\"Compat-wireless\"" \
        -DCOMPAT_VERSION="\"$(shell cat $(PWD)/$(src)/compat_version)\""

ccflags-y += -I../external/compat-wireless/include
ccflags-y += -include include/linux/compat-2.6.h
ccflags-y += -include ../../$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/linux/version.h

endif
