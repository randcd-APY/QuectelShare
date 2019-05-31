#if ACTUATOR_SUPPORTED set to none then no actuator library will compile
#if ACTUATOR_SUPPORTED is unassigned then all library will compile
#if ACTUATOR_SUPPORTED is set some to specific hardware  then only that will compile

CURR_PATH := $(call my-dir)
actuator :=
temp     :=
ifeq ($(TARGET_ARCH),$(filter $(TARGET_ARCH),arm arm64))
ifneq ($(ACTUATOR_SUPPORTED),)
ifneq ($(ACTUATOR_SUPPORTED),"none")
ACTUATOR_LIST := $(shell find $(CURR_PATH) -type d | sed s:^$(CURR_PATH)::g | sed s:^/::g)
$(foreach actuator,$(ACTUATOR_SUPPORTED),$(eval temp += $(filter $(actuator) $(actuator)%,$(ACTUATOR_LIST))))
$(foreach actuator,$(temp),$(eval include $(CURR_PATH)/$(actuator)/Android.mk))
endif
else
include $(call all-subdir-makefiles)
endif
endif
