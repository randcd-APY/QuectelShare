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

my $currentyear = (localtime)[5] + 1900;
open (PAT_FILE, "<:raw", $ARGV[0]) or die "Unable to open $ARGV[0]...";
binmode PAT_FILE;
my ($outFileName);
$outFileName = $ARGV[0];
$outFileName =~ s/^(.*).pat/$1.c/ or die "File must be of type .pat";
open (C_FILE, ">$outFileName") or die "Unable to open output file $outFileName";
my ($nameTag);
$nameTag = $ARGV[0];
$nameTag =~ s/^.*[\/\\](.*).pat/$1/;
$nameTag =~ s/[\$#@~!&*()\[\];.,:?^ `\\\/]+/_/g;
$nameTag =~ s/-/_/g;
$nameTag =~ s/ /_/g;
$nameTag =~ s/\./_/;
my $fileSize = -s $ARGV[0];
print "Converting $ARGV[0] to $outFileName, ";
print C_FILE "/******************************************************************************\n";
print C_FILE "*\n";
print C_FILE "*        Copyright Cambridge Silicon Radio Limited and its licensors $currentyear.\n";
print C_FILE "*        All rights reserved.\n";
print C_FILE "*\n";
print C_FILE "******************************************************************************/\n";
print C_FILE "/* Note: this is an auto-generated file. */\n";
print C_FILE "\n";
print C_FILE "#include \"csr_synergy.h\"\n";
print C_FILE "#include \"csr_types.h\"\n";
print C_FILE "\n";
print C_FILE "const CsrUint8 $nameTag\[\] = \n";
print C_FILE "{\n";

my $bytesToProcess = 16;
my $oneByte;
print "Size of the PAT file is : ";
print $fileSize--;
print " bytes";
print "\n";
while (read(PAT_FILE, $oneByte, 1))
{
    print C_FILE sprintf("  0x%02x", ord $oneByte);
	$bytesToProcess --;

	if ($fileSize--)
	{
		print C_FILE ",";
	}

    if ($bytesToProcess == 0)
    {
	    print C_FILE "\n";
	    $bytesToProcess = 16;
	}
}

print C_FILE "}";
print C_FILE ";\n\n";

print C_FILE "const CsrUint32 $nameTag\_length\[\] = {sizeof($nameTag)};\n\n";

close C_FILE;
close PAT_FILE;
