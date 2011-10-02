#!/usr/bin/perl

use lib '/usr/share/sax/profile';

use strict;
use Profile;

#====================================
# Profile name...
#------------------------------------
my $name = "FGLRX_DualHead_DriverOptions";

#====================================
# Init profile script
#------------------------------------
my $profile = ProfileInitScript();

#====================================
# Do the profile adaptions...
#------------------------------------
if (ProfileIsNoteBookHardware()) {
	open (FD,">",$profile) ||
		die "$name: Can't open $profile: $!";
	my $id = 55;
	my $dt = '"Device->[X]->Raw->".$id++."->Option"';
	my $dx = eval $dt;
	print FD "SaXMeta->[X]->SAX_NO_CDB_CHECK=1\n";
	print FD "Device->[X]->Option=SaXDualHead\n";
	print FD "$dx=\"DesktopSetup\" \"Clone\"\n";
	$dx = eval $dt;
	print FD "$dx=\"ForceMonitors\" \"auto,crt1\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualOrientation\" \"LeftOf\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualMode\" \"Clone\"\n";
	$dx = eval $dt;
	print FD "$dx=\"Mode2\" \"1024x768\"\n";
	$dx = eval $dt;
	print FD "$dx=\"HSync2\" \"31-48\"\n";
	$dx = eval $dt;
	print FD "$dx=\"VRefresh2\" \"50-60\"\n";
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
	close FD;
} else {
	#====================================
	# check secondary DDC data
	#------------------------------------
	open (FD,">>",$profile) ||
		die "$name: Can't open $profile: $!";
	my %data   = ProfileGetDDC2Data();
	my $id = 55;
	my $dt = '"Device->[X]->Raw->".$id++."->Option"';
	my $dx = eval $dt;
	print FD "$dx=\"DesktopSetup\" \"Clone\"\n";
	$dx = eval $dt;
	print FD "$dx=\"ForceMonitors\" \"auto,crt1\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualOrientation\" \"LeftOf\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualMode\" \"Clone\"\n";
	$dx = eval $dt;
	foreach my $key (keys %data) {
		my $val = $data{$key};
		SWITCH: for ($key) {
			/^Model/      && do {
				$id = 66;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualMonitorModel\" \"$val\"\n";
				last SWITCH;
			};
			/^Vendor/     && do {
				$id = 65;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualMonitorVendor\" \"$val\"\n";
				last SWITCH;
			};
			/^Resolution/ && do {
				$id = 59;
				$dx = eval $dt;
				print FD "$dx=\"Mode2\" \"$val\"\n";
				$id = 64;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualResolution\" \"$val\"\n";
				last SWITCH;
			};
			/^Hsync/      && do {
				$id = 60;
				$dx = eval $dt;
				print FD "$dx=\"HSync2\" \"$val\"\n";
				$id = 62;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualHSync\" \"$val\"\n";
				last SWITCH;
			};
			/^Vsync/      && do {
				$id = 61;
				$dx = eval $dt;
				print FD "$dx=\"VRefresh2\" \"$val\"\n";
				$id = 63;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualVSync\" \"$val\"\n";
				last SWITCH;
			};
		}
	}
	close FD;
}
