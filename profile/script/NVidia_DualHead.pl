#!/usr/bin/perl

use lib '/usr/share/sax/profile';

use strict;
use Profile;

#====================================
# DualHead check
#------------------------------------
if ($ARGV[0] eq "check") {
	ProfileNVDualCheck();
	exit 0;
}

#====================================
# Init profile script
#------------------------------------
my $profile = ProfileInitScript();

#====================================
# Do the profile adaptions...
#------------------------------------
if (ProfileIsXOrgVendor ("nvidia")) {
	#====================================
	# nvidia binary driver not installed
	#------------------------------------
	open (FD,">",$profile) ||
		die "NVidia_DualHead: Can't open $profile: $!";
	print FD "Desktop -> [X] ->  CalcModelines = yes\n";
	print FD "Monitor -> [X] ->  CalcAlgorithm = CheckDesktopGeometry\n";
	close FD;
} else {
	#====================================
	# check secondary DDC data
	#------------------------------------
	open (FD,">>",$profile) ||
		die "NVidia_DualHead: Can't open $profile: $!";
	my %data = ProfileGetDDC2Data();
	foreach my $key (keys %data) {
		my $val = $data{$key};
		SWITCH: for ($key) {
			/^Model/      && do {
				print FD "Monitor -> [X] -> ModelName = $val\n";
				last SWITCH;
			};
			/^Vendor/     && do {
				print FD "Monitor -> [X] -> VendorName = $val\n";
				last SWITCH;
			};
			/^Resolution/ && do {
				foreach my $depth (8,15,16,24) {
					print FD "Screen -> [X] -> Depth->$depth->Modes = $val\n";
				}
				last SWITCH;
			};
			/^Hsync/      && do {
				print FD "Monitor -> [X] -> HorizSync = $val\n";
				last SWITCH;
			};
			/^Vsync/      && do {
				print FD "Monitor -> [X] -> VertRefresh = $val\n";
				last SWITCH;
			};
			/^Size/       && do {
				print FD "Monitor -> [X] -> DisplaySize = $val\n";
				last SWITCH;
			};
		}
	}
	close FD;
}
