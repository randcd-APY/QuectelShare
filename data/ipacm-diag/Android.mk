BOARD_PLATFORM_LIST := msm8994
BOARD_PLATFORM_LIST += msm8992
BOARD_PLATFORM_LIST += msm8952
BOARD_PLATFORM_LIST += msm8996
BOARD_PLATFORM_LIST += msm8953
ifeq ($(call is-board-platform-in-list,$(BOARD_PLATFORM_LIST)),true)
include $(call all-subdir-makefiles)
endif # QCOM_TARGET_PRODUCT
