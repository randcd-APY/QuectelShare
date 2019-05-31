#!/usr/bin/perl
#############################################################################
#
#    Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#       
#############################################################################
use warnings;
use strict;
my $cfg_file = shift(@ARGV);
my $rules_file = shift(@ARGV);
open(my $in,'<',$cfg_file) or die "could not open $cfg_file\n";
open(my $out,'>',$rules_file) or die "could not open $rules_file\n";
while (<$in>)
{
    my $line = $_;
    if ($line =~ /\s*(\w+)\s*(:=)\s*((\w|\.)+)/)
    {
        my $key = $1;
        my $val = $3;
        my $op = $2;

        if ($key !~ /(_ROOT|_PATH)/)
        {
            print $out "#ifndef $key\n";
            print $out "#define $key \t\t \($val\)\n";
            print $out "#endif\n\n";
        }
    }
}
