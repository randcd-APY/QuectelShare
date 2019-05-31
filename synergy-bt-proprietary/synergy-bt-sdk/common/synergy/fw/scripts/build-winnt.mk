##############################################################################
#
# Copyright (c) 2008-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################
RM := - del /f
CP := - copy
MV := - move
MKDIR := - mkdir
ECHO := echo
FIND := dir /s /b 
path_subst = $(subst /,\,$1)

USE_DEPENDENCY_FILES := 0

