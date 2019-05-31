CURR_PATH := $(call my-dir)
sensor :=
temp   :=
ifeq ($(TARGET_ARCH),$(filter $(TARGET_ARCH),arm arm64))
ifneq ($(SENSOR_SUPPORTED),)
SENSOR_LIST := $(shell find $(CURR_PATH) -type d | sed s:^$(CURR_PATH)::g | sed s:^/::g)
$(foreach sensor,$(SENSOR_SUPPORTED),$(eval temp += $(filter $(sensor) $(sensor)%,$(SENSOR_LIST))))
$(foreach sensor,$(temp),$(eval include $(CURR_PATH)/$(sensor)/Android.mk))
else
include $(call all-subdir-makefiles)
endif
endif
