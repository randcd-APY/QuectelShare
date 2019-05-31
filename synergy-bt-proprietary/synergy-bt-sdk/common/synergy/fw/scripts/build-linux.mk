##############################################################################
#
# Copyright (c) 2008-2015 Qualcomm Technologies International, Ltd.
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
##############################################################################
RM := -/bin/rm -f
CP := -/bin/cp
MV := -/bin/mv
MKDIR := -/bin/mkdir -p
ECHO := echo
FIND := find
AWK := gawk
TAR := tar
GREP := grep
PERL := /usr/bin/perl
RMDIR := -rmdir

path_subst = $(subst \,/,$1)

