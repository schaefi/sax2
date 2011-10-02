#!/usr/bin/perl

use lib '/usr/share/sax/profile';

use strict;
use Profile;

#====================================
# Init profile script
#------------------------------------
my $profile = ProfileInitScript();

#=====================================
# get SYSP xstuff data
#-------------------------------------
my $outputName;
my $xstuff = new SaX::SaXImportSysp ($SaX::SYSP_DESKTOP);
$xstuff->doImport();
my $rroutput = $xstuff->getItem("RandR");
my $randrok  = 1;
if ((! defined $rroutput) || ($rroutput eq "<undefined>")) {
	$randrok = 0;
}
if ($randrok) {
	#=====================================
	# use first randr plugin as primary
	#-------------------------------------
	my @channels = split ("%",$rroutput);
	foreach my $channel (@channels) {
		my @data = split (" ",$channel);
		my $outputState = $data[1];
		if ($outputState ne "connected") {
			next;
		}
		$outputName = $data[0];
		last;
	}
}
#====================================
# Do the profile adaptions...
#------------------------------------
open (FD,">",$profile) ||
	die "RadeonHD: Can't open $profile: $!";
if (defined $outputName) {
	print FD "Device -> [X] -> Raw -> 18 -> Option = ";
	print FD "\"monitor-$outputName\" \"Monitor[0]\"\n";
}
close FD;
