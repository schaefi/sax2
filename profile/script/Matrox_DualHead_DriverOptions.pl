#!/usr/bin/perl

use lib '/usr/share/sax/profile';

use strict;
use Profile;

#====================================
# Profile name...
#------------------------------------
my $name = "Matrox_DualHead_DriverOptions";

#====================================
# Init profile script
#------------------------------------
my $profile = ProfileInitScript();

#====================================
# Do the profile adaptions...
#------------------------------------
#====================================
# check secondary DDC data
#------------------------------------
open (FD,">>",$profile) ||
	die "$name: Can't open $profile: $!";
my %data   = ProfileGetDDC2Data();
my $id = 20;
my $dt = '"Device->[X]->Raw->".$id++."->Option"';
my $dx = eval $dt;
print FD "$dx=\"MergedFB\" \"yes\"\n";
$dx = eval $dt;
print FD "$dx=\"Monitor2Position\" \"Clone\"\n";
$dx = eval $dt;
print FD "$dx=\"SaXDualOrientation\" \"LeftOf\"\n";
$dx = eval $dt;
print FD "$dx=\"SaXDualMode\" \"Clone\"\n";
$dx = eval $dt;
foreach my $key (keys %data) {
	my $val = $data{$key};
	SWITCH: for ($key) {
		/^Model/      && do {
			$id = 31;
			$dx = eval $dt;
			print FD "$dx=\"SaXDualMonitorModel\" \"$val\"\n";
			last SWITCH;
		};
		/^Vendor/     && do {
			$id = 30;
			$dx = eval $dt;
			print FD "$dx=\"SaXDualMonitorVendor\" \"$val\"\n";
			last SWITCH;
		};
		/^Resolution/ && do {
			$id = 26;
			$dx = eval $dt;
			print FD "$dx=\"MetaModes\" \"\${Modes[0]}-$val\"\n";
			$id = 29;
			$dx = eval $dt;
			print FD "$dx=\"SaXDualResolution\" \"$val\"\n";
			last SWITCH;
		};
		/^Hsync/      && do {
			$id = 24;
			$dx = eval $dt;
			print FD "$dx=\"Monitor2HSync\" \"$val\"\n";
			$id = 27;
			$dx = eval $dt;
			print FD "$dx=\"SaXDualHSync\" \"$val\"\n";
			last SWITCH;
		};
		/^Vsync/      && do {
			$id = 25;
			$dx = eval $dt;
			print FD "$dx=\"Monitor2VRefresh\" \"$val\"\n";
			$id = 28;
			$dx = eval $dt;
			print FD "$dx=\"SaXDualVSync\" \"$val\"\n";
			last SWITCH;
		};
	}
}
close FD;
