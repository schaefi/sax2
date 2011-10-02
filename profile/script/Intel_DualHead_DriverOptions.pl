#!/usr/bin/perl

use lib '/usr/share/sax/profile';

use strict;
use Profile;

#====================================
# Profile name...
#------------------------------------
my $name = "Intel_DualHead_DriverOptions";

#====================================
# Init profile script
#------------------------------------
my $profile = ProfileInitScript();

#====================================
# Do the profile adaptions...
#------------------------------------
my $mlayout = ProfileIntelSetupMonitorLayout ($profile);

#====================================
# Do the profile adaptions...
#------------------------------------
if (ProfileIsNoteBookHardware()) {
	open (FD,">",$profile) ||
		die "$name: Can't open $profile: $!";
	my $id = 20;
	my $dt = '"Device->[X]->Raw->".$id++."->Option"';
	my $dx = eval $dt;
	print FD "SaXMeta->[X]->SAX_NO_CDB_CHECK=1\n";
	print FD "\$MS=Screen->[X]->Depth->16->Modes\n";
	print FD "Device->[X]->Option=SaXDualHead\n";
	print FD "$dx=\"MonitorLayout\" \"$mlayout\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualMode\" \"Clone\"\n";
	$dx = eval $dt;
	print FD "$dx=\"Clone\" \"yes\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualOrientation\" \"LeftOf\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualHSync\" \"31-48\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualVSync\" \"50-60\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualResolution\" \"1024x768\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualMonitorVendor\" \"__VESA__\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualMonitorModel\" \"1024X768\@60HZ\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SecondMonitorHorizSync\" \"31-48\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SecondMonitorVertRefresh\" \"50-60\"\n";
	$dx = eval $dt;
	print FD "$dx=\"MetaModes\" \"\${MS[0]}-1024x768;1024x768-1024x768\"\n";
	close FD;
} else {
	#====================================
	# check secondary DDC data
	#------------------------------------
	open (FD,">>",$profile) ||
		die "$name: Can't open $profile: $!";
	my %data   = ProfileGetDDC2Data();
	my $id = 20;
	my $dt = '"Device->[X]->Raw->".$id++."->Option"';
	my $dx = eval $dt;
	print FD "$dx=\"MonitorLayout\" \"$mlayout\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualMode\" \"Clone\"\n";
	$dx = eval $dt;
	print FD "$dx=\"Clone\" \"yes\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualOrientation\" \"LeftOf\"\n";
	$dx = eval $dt;
	foreach my $key (keys %data) {
		my $val = $data{$key};
		SWITCH: for ($key) {
			/^Model/      && do {
				$id = 28;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualMonitorModel\" \"$val\"\n";
				last SWITCH;
			};
			/^Vendor/     && do {
				$id = 27;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualMonitorVendor\" \"$val\"\n";
				last SWITCH;
			};
			/^Resolution/ && do {
				$id = 31;
				$dx = eval $dt;
				print FD "$dx=\"MetaModes\" \"\${Modes[0]}-$val\"\n";
				$id = 26;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualResolution\" \"$val\"\n";
				last SWITCH;
			};
			/^Hsync/      && do {
				$id = 29;
				$dx = eval $dt;
				print FD "$dx=\"SecondMonitorHorizSync\" \"$val\"\n";
				$id = 24;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualHSync\" \"$val\"\n";
				last SWITCH;
			};
			/^Vsync/      && do {
				$id = 30;
				$dx = eval $dt;
				print FD "$dx=\"SecondMonitorVertRefresh\" \"$val\"\n";
				$id = 25;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualVSync\" \"$val\"\n";
				last SWITCH;
			};
		}
	}
	close FD;
}
