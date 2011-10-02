#!/usr/bin/perl

use lib '/usr/share/sax/profile';

use strict;
use Profile;

#====================================
# Profile name...
#------------------------------------
my $name = "Radeon_DualHead_DriverOptions";

#====================================
# Init profile script
#------------------------------------
my $profile = ProfileInitScript();

#====================================
# get DisplaySize for second channel
#------------------------------------
my %data = ProfileGetDDC2Data();
my $Size;
foreach my $key (keys %data) {
	my $val = $data{$key};
	SWITCH: for ($key) {
		/^Size/       && do {
			$Size = $val;
			last SWITCH;
		};
	}
}
#=====================================
# get SYSP xstuff data
#-------------------------------------
my $xstuff = new SaX::SaXImportSysp ($SaX::SYSP_DESKTOP);
$xstuff->doImport();
my $rroutput = $xstuff->getItem("RandR");
my $randrok  = 1;
if ((! defined $rroutput) || ($rroutput eq "<undefined>")) {
	$randrok = 0;
}
#====================================
# Do the profile adaptions...
#------------------------------------
if (ProfileIsNoteBookHardware()) {
	open (FD,">",$profile) ||
		die "$name: Can't open $profile: $!";
	my $id = 21;
	my $dt = '"Device->[X]->Raw->".$id++."->Option"';
	my $dx = eval $dt;
	print FD "SaXMeta->[X]->SAX_NO_CDB_CHECK=1\n";
	print FD "\$MS=Screen->[X]->Depth->16->Modes\n";
	print FD "\$Hsync=Monitor->[X]->HorizSync\n";
	print FD "\$Vsync=Monitor->[X]->VertRefresh\n";
	print FD "Device->[X]->Option=SaXDualHead\n";
	print FD "$dx=\"SaXDualMode\" \"Clone\"\n";
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
	print FD "$dx=\"SaXExternal\" \"Identifier__AND__EXT__PLUS__VertRefresh__AND__50-60__PLUS__HorizSync__AND__31-48__PLUS__PreferredMode__AND__1024x768__PLUS__VendorName__AND____VESA____PLUS__ModelName__AND__1024X768\@60HZ";
	if (defined $Size) {
		print FD "__PLUS__DisplaySize__AND__$Size\"\n";
	} else {
		print FD "\"\n";
	}
	if ($randrok) {
		#=====================================
		# setup randr plugin data
		#-------------------------------------
		my @sortedChannels = ProfileSortRandrData ($rroutput);
		foreach my $outputName (@sortedChannels) {
			if ((defined $outputName) && ($outputName ne "")) {
				$dx = eval $dt;
				print FD "$dx=\"monitor-$outputName\" \"EXT\"\n";
			}
		}
	}
	close FD;
} else {
	#====================================
	# check secondary DDC data
	#------------------------------------
	open (FD,">>",$profile) ||
		die "$name: Can't open $profile: $!";
	my %data   = ProfileGetDDC2Data();
	my $id = 21;
	my $dt = '"Device->[X]->Raw->".$id++."->Option"';
	my $dx = eval $dt;
	print FD "$dx=\"SaXDualMode\" \"Clone\"\n";
	$dx = eval $dt;
	print FD "$dx=\"SaXDualOrientation\" \"LeftOf\"\n";
	$dx = eval $dt;
	my $Hsync  = '${Hsync}';
	my $Vsync  = '${Vsync}';
	my $Vendor = '${Vendor}';
	my $Model  = '${Model}'; 
	my $Modes  = '${Modes[0]}';
	foreach my $key (keys %data) {
		my $val = $data{$key};
		SWITCH: for ($key) {
			/^Model/      && do {
				$id = 27;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualMonitorModel\" \"$val\"\n";
				$Model = $val;
				last SWITCH;
			};
			/^Vendor/     && do {
				$id = 26;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualMonitorVendor\" \"$val\"\n";
				$Vendor = $val;
				last SWITCH;
			};
			/^Resolution/ && do {
				$id = 25;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualResolution\" \"$val\"\n";
				$Modes = $val;
				last SWITCH;
			};
			/^Hsync/      && do {
				$id = 23;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualHSync\" \"$val\"\n";
				$Hsync = $val;
				last SWITCH;
			};
			/^Vsync/      && do {
				$id = 24;
				$dx = eval $dt;
				print FD "$dx=\"SaXDualVSync\" \"$val\"\n";
				$Vsync = $val;
				last SWITCH;
			};
		}
	}
	$id = 28;
	$dx = eval $dt;
	print FD "$dx=\"SaXExternal\" \"Identifier__AND__EXT__PLUS__VertRefresh__AND__".$Vsync."__PLUS__HorizSync__AND__".$Hsync."__PLUS__PreferredMode__AND__".$Modes."__PLUS__VendorName__AND__".$Vendor."__PLUS__ModelName__AND__".$Model;
	if (defined $Size) {
		print FD "__PLUS__DisplaySize__AND__$Size\"\n";
	} else {
		print FD "\"\n";
	}
	if ($randrok) {
		#=====================================
		# setup randr plugin data
		#-------------------------------------
		my @sortedChannels = ProfileSortRandrData ($rroutput);
		foreach my $outputName (@sortedChannels) {
			if ((defined $outputName) && ($outputName ne "")) {
				$dx = eval $dt;
				print FD "$dx=\"monitor-$outputName\" \"EXT\"\n";
			}
		}
	}
	close FD;
}
