ifeq ($(BOARD_WLAN_DEVICE),qcwcn)
    include $(call all-subdir-makefiles)
    LOCAL_CFLAGS += -Wall -Werror
endif
