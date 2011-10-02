#!/usr/bin/perl

use lib '/usr/share/sax/profile';
use lib '.';

use strict;
use Profile;

#====================================
# Profile name...
#------------------------------------
my $name = "NVidia_DualHead_DriverOptions";

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
	open (FD,">",$profile) ||
		die "$name: Can't open $profile: $!";
	print FD "Desktop -> [X] -> CalcModelines = yes\n";
	print FD "Monitor -> [X] -> CalcAlgorithm = CheckDesktopGeometry\n";
	close FD;
} else {
	#====================================
	# Do the profile adaptions...
	#------------------------------------
	my $mlayout = ProfileNVidiaSetupMonitorLayout ($profile);
	my ($CA,$CB) = split (/,/,$mlayout);
	if (ProfileIsNoteBookHardware()) {
		open (FD,">",$profile) ||
			die "$name: Can't open $profile: $!";
		my $id = 11;
        my $dt = '"Device->[X]->Raw->".$id++."->Option"';
        my $dx = eval $dt;
		print FD "\$MS=Screen->[X]->Depth->16->Modes\n";
		print FD "SaXMeta->[X]->SAX_NO_CDB_CHECK=1\n";
		print FD "Desktop->[X]->CalcModelines=no\n";
		print FD "Monitor->[X]->CalcAlgorithm=XServerPool\n";
		print FD "Device->[X]->Driver=nvidia\n";
		print FD "Device->[X]->Screen=0\n";
		print FD "Device->[X]->Option = TwinView,SaXDualHead\n";
		print FD "$dx=\"TwinViewOrientation\" \"Clone\"\n";
		$dx = eval $dt;
		print FD "$dx=\"ConnectedMonitor\" \"$mlayout\"\n";
		$dx = eval $dt;
		print FD "$dx=\"SaXDualOrientation\" \"RightOf\"\n";
		$dx = eval $dt;
		print FD "$dx=\"SaXDualMode\" \"Clone\"\n";
		$dx = eval $dt;
		print FD "$dx=\"SecondMonitorHorizSync\" \"31-48\"\n";
		$dx = eval $dt;
		print FD "$dx=\"SecondMonitorVertRefresh\" \"50-60\"\n";
		$dx = eval $dt;
		my $MMValue = "$CA:\${MS[0]},$CB:1024x768;$CA:1024x768,$CB:1024x768";
		print FD "$dx=\"MetaModes\" \"$MMValue\"\n";
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
		print FD "$dx=\"TwinViewXineramaInfoOrder\" \"$mlayout\"\n";
		close FD;
	} else {
		#====================================
		# check secondary DDC data
		#------------------------------------
		open (FD,">>",$profile) ||
			die "$name: Can't open $profile: $!";
		my %data   = ProfileGetDDC2Data();
		my $id = 11;
		my $dt = '"Device->[X]->Raw->".$id++."->Option"';
		my $dx = eval $dt;
		print FD "$dx=\"TwinViewOrientation\" \"Clone\"\n";
		$dx = eval $dt;
		print FD "$dx=\"ConnectedMonitor\" \"$mlayout\"\n";
		$dx = eval $dt;
		print FD "$dx=\"SaXDualOrientation\" \"RightOf\"\n";
		$dx = eval $dt;
		print FD "$dx=\"SaXDualMode\" \"Clone\"\n";
		$dx = eval $dt;
		print FD "$dx=\"TwinViewXineramaInfoOrder\" \"$mlayout\"\n";
		$dx = eval $dt;
		foreach my $key (keys %data) {
			my $val = $data{$key};
			SWITCH: for ($key) {
				/^Model/      && do {
					$id = 22;
					$dx = eval $dt;
					print FD "$dx=\"SaXDualMonitorModel\" \"$val\"\n";
					last SWITCH;
				};
				/^Vendor/     && do {
					$id = 21;
					$dx = eval $dt;
					print FD "$dx=\"SaXDualMonitorVendor\" \"$val\"\n";
					last SWITCH;
				};
				/^Resolution/ && do {
					$id = 17;
					$dx = eval $dt;
					my $key = "MetaModes";
					print FD "$dx=\"$key\" \"$CA:\${Modes[0]},$CB:$val\"\n";
					$id = 20;
					$dx = eval $dt;
					print FD "$dx=\"SaXDualResolution\" \"$val\"\n";
					last SWITCH;
				};
				/^Hsync/      && do {
					$id = 15;
					$dx = eval $dt;
					print FD "$dx=\"SecondMonitorHorizSync\" \"$val\"\n";
					$id = 18;
					$dx = eval $dt;
					print FD "$dx=\"SaXDualHSync\" \"$val\"\n";
					last SWITCH;
				};
				/^Vsync/      && do {
					$id = 16;
					$dx = eval $dt;
					print FD "$dx=\"SecondMonitorVertRefresh\" \"$val\"\n";
					$id = 19;
					$dx = eval $dt;
					print FD "$dx=\"SaXDualVSync\" \"$val\"\n";
					last SWITCH;
				};
			}
		}
		close FD;
	}
}
