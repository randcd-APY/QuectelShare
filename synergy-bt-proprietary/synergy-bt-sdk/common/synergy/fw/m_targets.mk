ifeq ($(USE_DEPENDENCY_FILES),1)
##############################################################################
#
# Copyright (c) 2008-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################
ifneq ($(DEP),)
$(OBJ_PATH)/%.d: %.c
	$(MKDIR) $(@D)
	gcc -MM -D_DEP_BUILD_ $(CFLAGS) $(EXTRA_CFLAGS) $(INC) $< -MT $(@) -MT $(@:.d=.o) -MP | sed -e 's,\(.\):\/,\/cygdrive\/\1\/,g' > $@

$(OBJ_PATH)/%.d: %.cpp
	$(MKDIR) $(@D)
	gcc -MM -D_DEP_BUILD_ $(CFLAGS) $(EXTRA_CFLAGS) $(INC) $< -MT $(@) -MT $(@:.d=.o) -MP | sed -e 's,\(.\):\/,\/cygdrive\/\1\/,g' > $@

#
# Include dependency files if any exists
#
-include $(DEP)
endif
endif

m_target_debug:
	@echo -e "TARGET        = $(TARGET)"
	@echo -e "OBJ_PATH      = $(OBJ_PATH)"
	@echo -e "INC           = $(INC)"
	@echo -e "CFLAGS        = $(CFLAGS)"
	@echo -e "EXTRA_CFLAGS  = $(EXTRA_CFLAGS)"
	@echo -e "TARGET_CFLAGS = $(TARGET_CFLAGS)"
	@echo -e "USE_DEPENDENCY_FILES = $(USE_DEPENDENCY_FILES)"
	@echo -e "DEP           = $(DEP)"
