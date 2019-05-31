###############################################################################
#
# Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
# All Rights Reserved. 
# Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
# REVISION:      $Revision: #1 $
###############################################################################
use strict;
use warnings;

 
my @files = glob("*.pat");

foreach my $file (@files) {
    print "$file\n";
	system("perl ./pat2c.pl $file");
}
exit 0;

