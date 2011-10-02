#!/usr/bin/perl

use lib   '/usr/share/sax/modules';

use strict;
use CreateSections;
use Storable;
use SaX;

#=====================================
# ProfileGetDualDisplaySize
#-------------------------------------
sub ProfileGetDualDisplaySize {
	my $x1 = 0;
	my $x2 = 0;
	my $x  = 0;
	my $y  = 0;
	my $sysp = "/usr/sbin/sysp";
	my $s1 = qx ($sysp -q xstuff | grep Size | head -n 1 | cut -f2 -d:);
	my $s2 = qx ($sysp -q xstuff | grep Size | tail -n 1 | cut -f2 -d:);
	if ($s1 =~ /(\d+)x(\d+)/) {
		$x1 = $1;
		$y  = $2;
	}
	if ($s2 =~ /(\d+)x(\d+)/) {
		$x2 = $1;
	} else {
		$x2 = $x1;
	}
	$x = $x1 + $x2;
	return ($x,$y);
}

#=====================================
# ProfileIsNoteBookHardware
#-------------------------------------
sub ProfileIsNoteBookHardware {
	my $import = new SaX::SaXImport ( $SaX::SAX_CARD );
	my $name = $import->getSectionName();
	my $saxCard = new SaX::SaXManipulateCard (
		$import
	);
	if ($saxCard->isNoteBook()) {
		return 1;
	}
	return 0;
}

#=====================================
# ProfileName
#-------------------------------------
sub ProfileName {
	my $profile = qx(basename $0 | cut -f1 -d.);
	chomp $profile && return $profile;
}

#=====================================
# ProfileInitScript
#-------------------------------------
sub ProfileInitScript {
	my $profile = ProfileName();
	my $stdname = $profile;
	my $newfile = "/var/lib/sax/$profile.tmp";
	$profile = "/usr/share/sax/profile/$profile";
	if (( ! -f $profile ) || ( $< != 0 )) {
		die "*** $stdname: no such file or permission denied";
	}
	qx (cp $profile $newfile);
	return $newfile;
}

#=====================================
# ProfileIsXOrgVendor
#-------------------------------------
sub ProfileIsXOrgVendor {
	my $driver = $_[0];
	my $vendor = qx(/usr/share/sax/sysp/script/vendor.pl $driver);
	chomp $vendor;
	if (($vendor eq "The XFree86 Project") || ($vendor eq "X.Org Foundation")) {
		return 1;
	}
	return 0;
}

#=====================================
# ProfileNVDualCheck
#-------------------------------------
sub ProfileNVDualCheck {
	my $vendor = qx(/usr/share/sax/sysp/script/vendor.pl nvidia);
	chomp $vendor;
	if (($vendor eq "The XFree86 Project") || ($vendor eq "X.Org Foundation")) {
		print "single\n";
	} else {
		print "dual\n";
	}
}

#=====================================
# ProfileReadXLogFile
#-------------------------------------
sub ProfileReadXLogFile {
	my $stdname = ProfileName();
	my $xorglogname = "/var/log/Xorg.99.log";
	if (($ENV{HW_UPDATE} == 1) || (! -f $xorglogname)) {
		my $xc = ProfileCreatePreliminaryConfig();
		qx (Xorg -probeonly -logverbose 255 -xf86config $xc -br :99 >/dev/null 2>&1);
	}
	local $/;
	open (FD, "<$xorglogname") ||
		die "*** $stdname: Cannot read X.org log $xorglogname";
	my $xorglog = <FD>;
	close FD;
	return $xorglog;
}

#=====================================
# ProfileIntelGetMonitorLayout
#-------------------------------------
sub ProfileIntelGetMonitorLayout {
	my $xorglog  = ProfileReadXLogFile();
	my %bootdevs = ();
	my %devs = ();
	my $primary;
	my $secondary;
	my $connected;
	$_ = $xorglog;
	my $D = "Display Info:";
	my $A = "attached:";
	my $P = "present:";
	while (/^\(..\) I8.0\(\d+\): $D ([^ :]+)[^:]*: $A (\w+), $P (\w+)/mg) {
		my $d=$1;
		my $a=$2;
		my $p=$3;
		if ($p =~ /true/i) {
			$devs{$d}=1;
			$bootdevs{$d}=1 if $a =~ /true/i;
		}
	}
	print STDERR "*** Display info disabled - guessing displays\n";
	if (keys %devs < 1) {
		my $C = "Currently active displays on Pipe";
		my $p = "";
		while ($xorglog =~ /^\(..\) I8.0\(\d+\): (.*)$/mg) {
			$_=$1;
			$p="" unless /^\s/;
			$p=$1 if /^\s*$C\s+(\S+):\s*$/;
			if ($p ne "" && /^\s+(\S+)/) {
				$devs{$1}=1;
				$bootdevs{$1}=1 if $1 eq "LFP";
			}
		}
	}
	if ($bootdevs{"LFP"}) {
		$primary="LFP";
	} elsif ($bootdevs{"DFP"}) {
		$primary="DFP";
	} elsif ($bootdevs{"CRT"}) {
		$primary="CRT";
		print STDERR "*** Device not booted into DFP panel\n";
	} else {
		$primary="LFP";
		print STDERR "*** Cannot determine boot display\n";
	}
	undef $devs{$primary};
	if ($devs{"LFP"}) {
		$primary="LFP";
		$secondary="CRT";
		print STDERR "*** Not booted into LFP, but connected\n";
		print STDERR "*** Weird. Config may not work\n";
	} elsif ($devs{"CRT"}) {
		$secondary="CRT";
	} elsif ($devs{"DFP"}) {
		$secondary="DFP";
		print STDERR "*** Secondary output seems to be a flat panel as well\n";
	} elsif ($devs{"DFP2"}) {
		$secondary="DFP";
		print STDERR "*** Secondary output seems to be a flat panel as well\n";
	} elsif ($devs{"CRT2"}) {
		$secondary="CRT";
	} else {
		$secondary="CRT";
		print STDERR "*** No secondary output found\n";
		print STDERR "*** Config may not work\n";
	}
	# ...
	# Intel chips *always* seem to have the internal display attached
	# to pipe B. The following code *might* work well enough to find strange
	# hardware with LDP attached to Pipe A. Might.
	# ---
	$connected="$secondary,$primary";
	$_ = $xorglog;
	if (s/^.*\(..\) I8.0\([0-9]+\): Currently active displays on Pipe A:\s*//s
		&& s/\(..\) I8.0\([0-9]+\): \S.*$//s
	) {
		if (/\sLFP\s/) {
			$connected="$primary,$secondary";
		}
	}
	$_ = $xorglog;
	if (/^\(..\) I[89].0\([0-9]+\): Primary Pipe is A,/m) {
		$connected="$primary,$secondary";
	}
	print STDERR "*** Selecting $connected as monitor configuration.\n";
	return $connected;
}

#=====================================
# ProfileIntelSetupMonitorLayout
#-------------------------------------
sub ProfileIntelSetupMonitorLayout {
	my $profile = $_[0];
	my $stdname = ProfileName();
	local $/;
	open (FD,"<",$profile) ||
		die "*** $stdname: Can't open $profile: $!";
	my $profileData = <FD>;
	my $monitorLayout = ProfileIntelGetMonitorLayout();
	$profileData =~ s/\[MONITORLAYOUT\]/$monitorLayout/;
	close FD;
	open (FD,">",$profile) ||
		die "*** $stdname: Can't open $profile: $!";
	print FD $profileData;
	close FD;
	return $monitorLayout;
}

#=====================================
# ProfileRadeonGetMonitorLayout
#-------------------------------------
sub ProfileRadeonGetMonitorLayout {
	my $xorglog  = ProfileReadXLogFile();
	my %devs = ();
	my $secondary = "AUTO";
	$_ = $xorglog;
	while (/^\(..\) RADEON\(\d+\): Port(\d):\n Monitor\s.*\n Connector\s*--\s*(\S+)\s*$/mg) {
		my $p=$1;
		my $c=$2;
		$devs{$p}=$c;
	}
	if (! defined $devs{2}) {
		if (! defined $devs{1}) {
			print STDERR "*** No output configuration found\n";
		} else {
			print STDERR "*** No secondary output found\n";
		}
	} elsif ($devs{2} eq "VGA" || $devs{2} eq "DVI-A") {
		$secondary = "CRT";
	} elsif ($devs{1} eq "LVDS" && $devs{2} eq "DVI-I") {
		$secondary = "CRT";
	} elsif ($devs{2} eq "DVI-D") {
		$secondary = "TMDS";
	} else {
		print STDERR "*** Nontrivial output configuration $devs{1},$devs{2} found\n";
	}
	print STDERR "*** Selecting AUTOCRT,$secondary as monitor configuration.\n";
	return "AUTOCRT,$secondary";
}

#=====================================
# ProfileRadeonSetupMonitorLayout
#-------------------------------------
sub ProfileRadeonSetupMonitorLayout {
	my $profile = $_[0];
	my $stdname = ProfileName();
	local $/;
	open (FD,"<",$profile) ||
		die "*** $stdname: Can't open $profile: $!";
	my $profileData = <FD>;
	my $monitorLayout = ProfileRadeonGetMonitorLayout();
	$profileData =~ s/\[MONITORLAYOUT\]/$monitorLayout/;
	close FD;
	open (FD,">",$profile) ||
		die "*** $stdname: Can't open $profile: $!";
	print FD $profileData;
	close FD;
	return $monitorLayout;
}

#=====================================
# ProfileNVidiaGetMonitorLayout
#-------------------------------------
sub ProfileNVidiaGetMonitorLayout {
	my $xorglog = ProfileReadXLogFile();
	my $bootdev;
	my $otherdevs;
	my $connected;
	$_ = $xorglog;
	my $I = '^\(..\) NVIDIA\(\d+\): ';
	my $B = 'Boot display device\(?s?\)?: ';
	my $S = 'Supported display device\(?s?\)?: ';
	if (/$I$B(.*?)\s*$/m) {
		$bootdev = $1;
		if ($bootdev =~ /,\s*(DFP[^,]*)/) {
			$bootdev = $1;
		} elsif ($bootdev =~ /,\s*(CRT[^,]*)/) {
			$bootdev = $1;
		} else {
			$bootdev =~ s/,.*$//;
		}
	} else {
		$bootdev = "AUTO";
	}
	if (/$I$S(.*?)\s*$/m) {
		($otherdevs = $1) =~ s/$bootdev//;
	} else {
		$otherdevs="AUTO";
	}
	if ($bootdev =~ /CRT/) {
		print STDERR "*** Device booted into CRT.\n";
		print STDERR "*** OOPS! This might not be what you intended!\n";
	}
	if ($bootdev =~ /DFP/ || $bootdev =~ /CRT/) {
		if ($otherdevs =~ /DFP/) {
		if ($otherdevs =~ /CRT/) {
			print STDERR "*** Secondary output might be both: DFP or a CRT.\n";
			print STDERR "    Change AUTO to DFP or CRT to activate\n";
			print STDERR "    the output without hardware plugged in.\n";
			$connected = "$bootdev,AUTO";
		} else {
			$connected = "$bootdev,DFP";
		}
		} elsif ($otherdevs =~ /CRT/) {
			$connected = "$bootdev,CRT";
		} else {
			print STDERR "*** No known secondary output found.\n";
			$connected = "$bootdev,AUTO";
		}
	} else {
		print STDERR "*** Unknown boot display device.\n";
		$connected = "$bootdev,AUTO";
	}

	print STDERR "*** Selecting $connected as monitor configuration.\n";
	return $connected;
}

#=====================================
# ProfileNvidiaSetupMonitorLayout
#-------------------------------------
sub ProfileNVidiaSetupMonitorLayout {
	my $profile = $_[0];
	my $stdname = ProfileName();
	local $/;
	open (FD,"<",$profile) ||
		die "*** $stdname: Can't open $profile: $!";
	my $profileData = <FD>;
	my $monitorLayout = ProfileNVidiaGetMonitorLayout();
	$profileData =~ s/\[MONITORLAYOUT\]/$monitorLayout/g;
	close FD;
	open (FD,">",$profile) ||
		die "*** $stdname: Can't open $profile: $!";
	print FD $profileData;
	close FD;
	return $monitorLayout;
}

#=====================================
# ProfileCreatePreliminaryConfig
#-------------------------------------
sub ProfileCreatePreliminaryConfig {
	my $cfgfile = "/tmp/xorg.conf.$$";
	my $config  = "/var/cache/sax/files/config";
	my $stdname = ProfileName();

	#==========================================
	# Retrieve registry data
	#------------------------------------------
	my $hashref = retrieve($config);
	if (! defined $hashref) {
		die "*** $stdname: can not load sax registry";
	}
	my %var = %{$hashref};

	#==========================================
	# Create config suggestion
	#------------------------------------------
	my @part0  = CreateHeaderSection ();
	my @part1  = CreateFilesSection  (\%var);
	my @part2  = CreateModuleSection (\%var);
	my @part3  = CreateServerFlagsSection (\%var);
	my @part4  = CreateInputDeviceSection (\%var);
	my @part5  = CreateMonitorSection (\%var,"yes");
	my @part7  = CreateDeviceSection (\%var);
	my @part8  = CreateScreenSection (\%var);
	my @part9  = CreateServerLayoutSection (\%var);
	my @part10 = CreateDRISection ();
	my @part11 = CreateExtensionsSection (\%var);

	#==========================================
	# Write preliminary config file
	#------------------------------------------
	open (HANDLE,">$cfgfile") ||
		die "*** $stdname: Can't open file: $cfgfile : $!";
	print HANDLE @part0;  print HANDLE "\n";
	print HANDLE @part1;  print HANDLE "\n";
	print HANDLE @part2;  print HANDLE "\n";
	print HANDLE @part3;  print HANDLE "\n";
	print HANDLE @part4;  print HANDLE "\n";
	print HANDLE @part5;  print HANDLE "\n";
	print HANDLE @part7;  print HANDLE "\n";
	print HANDLE @part8;  print HANDLE "\n";
	print HANDLE @part9;  print HANDLE "\n";
	print HANDLE @part10; print HANDLE "\n";
	print HANDLE @part11; print HANDLE "\n";
	close HANDLE;

	return $cfgfile;
}

#=====================================
# ProfileGetRealMouseDevice
#-------------------------------------
sub ProfileGetRealMouseDevice {
	my $card = $_[0];
	#=====================================
	# get SYSP mouse...
	#-------------------------------------
	my $mouse = new SaX::SaXImportSysp ($SaX::SYSP_MOUSE);
	$mouse->doImport();
	$mouse->setID ($card);
	my $device = $mouse->getItem("RealDevice");
	if (! defined $device) {
		return;
	}
	return $device;
}

#=====================================
# ProfileGetDDC2Data
#-------------------------------------
sub ProfileGetDDC2Data {
	my %result = ();
	#=====================================
	# get SYSP xstuff ...[2] data
	#-------------------------------------
	my $xstuff = new SaX::SaXImportSysp ($SaX::SYSP_DESKTOP);
	$xstuff->doImport();
	my $ddc = $xstuff->getItem("DDC[2]");
	if ((! defined $ddc) || ($ddc eq "")) {
		return %result;
	}
	#=====================================
	# Save DDC record values
	#-------------------------------------
	if (defined $xstuff->getItem("Vendor[2]")) {
	if (defined $xstuff->getItem("Name[2]")) {
		$result{Model}  = $xstuff->getItem("Name[2]");
		$result{Vendor} = $xstuff->getItem("Vendor[2]");
	}
	}
	if (defined $xstuff->getItem("Vesa[2]")) {
	if ($xstuff->getItem("Vesa[2]") =~ /.*,(.*?) (.*) .* .*$/)	{
		$result{Resolution} = "$1x$2";
	}
	}
	if (defined $xstuff->getItem("Hsync[2]")) {
		$result{Hsync} = "31-".$xstuff->getItem("Hsync[2]");
	}
	if (defined $xstuff->getItem("Vsync[2]")) {
		$result{Vsync} = "50-".$xstuff->getItem("Vsync[2]");
	}
	if (defined $xstuff->getItem("Size[2]")) {
	if ($xstuff->getItem("Size[2]") =~ /(.*)x(.*)/) {
		$result{Size} = "$1 $2";
	}
	}
	#=====================================
	# get CDB monitor pointer
	#-------------------------------------
	my %section;
	my @importID = (
		$SaX::SAX_CARD, $SaX::SAX_DESKTOP, $SaX::SAX_PATH
	);
	foreach my $id (@importID) {
		my $import = new SaX::SaXImport ( $id );
		$import->setSource ( $SaX::SAX_AUTO_PROBE );
		$import->doImport();
		$section{$import->getSectionName()} = $import;
	}
	my $mDesktop = new SaX::SaXManipulateDesktop (
		$section{Desktop},$section{Card},$section{Path}
	);
	my %data = %{$mDesktop->getCDBMonitorIDData ($ddc)};
	if (%data) {
		#=====================================
		# Found monitor in the CDB
		#-------------------------------------
		if (defined $data{Name}) {
			$data{Name} =~ s/\(/__OB__/g;
			$data{Name} =~ s/\)/__CB__/g;
			if ($data{Name} =~ /(.*):(.*)/) {
				$result{Model}  = $2;
				$result{Vendor} = $1;
			}
		}
		if (defined $data{Resolution}) {
			$result{Resolution} = $data{Resolution};
		}
		if (defined $data{HorizSync}) {
			$result{Hsync} = $data{HorizSync};
		}
		if (defined $data{VertRefresh}) {
			$result{Vsync} = $data{VertRefresh};
		}
	}
	return %result;
}

#=====================================
# ProfileSortRandrData
#-------------------------------------
sub ProfileSortRandrData {
	my $rroutput = shift;
	my @channels = split ("%",$rroutput);
	my @sortedChannels = ();
	my $index = -1;
	# /.../
	# assume first channel is internal display
	# skip the internal display
	# ----
	foreach my $channel (@channels) {
		my @data = split (" ",$channel); $index++;
		if ( $data[1] ne "connected") {
			next;
		}
		delete $channels[$index];
		last;
	}
	$index = 9;
	foreach my $channel (@channels) {
		my @data = split (" ",$channel);
		my $outputName  = $data[0];
		my $outputState = $data[1];
		# /.../
		# activate all connected output channels
		# in a specific order
		# ----
		if ($outputState ne "connected") {
			next;
		}
		if ($outputName eq "LVDS") {
			$sortedChannels[0] = $outputName;
		}
		elsif ($outputName =~ /^LVDS-.*/) {
			$sortedChannels[1] = $outputName;
		}
		elsif ($outputName eq "DVI") {
			$sortedChannels[2] = $outputName;
		}
		elsif ($outputName =~ /^DVI-.*/) {
			$sortedChannels[3] = $outputName;
		}
		elsif ($outputName eq "TMDS") {
			$sortedChannels[4] = $outputName;
		}
		elsif ($outputName =~ /^TMDS-.*/) {
			$sortedChannels[5] = $outputName;
		}
		elsif ($outputName eq "VGA") {
			$sortedChannels[6] = $outputName;
		}
		elsif ($outputName =~ /^VGA-.*/) {
			$sortedChannels[7] = $outputName;
		}
		elsif ($outputName eq "TV") {
			$sortedChannels[8] = $outputName;
		} else {
			$sortedChannels[$index] = $outputName;
			$index++;
		}
	}
	return @sortedChannels;
}

#=====================================
# ProfileSortRandrData
#-------------------------------------
sub ProfileSortRandrDataForRadeonHD {
	my $rroutput = shift;
	my @channels = split ("%",$rroutput);
	my @sortedChannels = ();
	my $index = -1;
	# /.../
	# assume first channel is internal display
	# skip the internal display
	# ----
	foreach my $channel (@channels) {
		my @data = split (" ",$channel); $index++;
		if ( $data[1] ne "connected") {
			next;
		}
		delete $channels[$index];
		last;
	}
	$index = 10;
	foreach my $channel (@channels) {
		my @data = split (" ",$channel);
		my $outputName  = $data[0];
		my $outputState = $data[1];
		# /.../
		# activate all connected output channels
		# in a specific order
		# ----
		if ($outputState ne "connected") {
			next;
		}
		if ($outputName eq "PANEL") {
			$sortedChannels[0] = $outputName;
		}
		elsif ($outputName =~ /^DVI-I_1/) {
			$sortedChannels[1] = $outputName;
		}
		elsif ($outputName =~ /^DVI-I_2/) {
			$sortedChannels[2] = $outputName;
		}
		elsif ($outputName =~ /^DVI-D_1/) {
			$sortedChannels[3] = $outputName;
		}
		elsif ($outputName =~ /^DVI-D_2/) {
			$sortedChannels[4] = $outputName;
		}
		elsif ($outputName =~ /^DVI-I/) {
			$sortedChannels[5] = $outputName;
		}
		elsif ($outputName =~ /^DVI-D/) {
			$sortedChannels[6] = $outputName;
		}
		elsif ($outputName =~ /^VGA_1/) {
			$sortedChannels[7] = $outputName;
		}
		elsif ($outputName =~ /^VGA_2/) {
			$sortedChannels[8] = $outputName;
		}
		elsif ($outputName =~ /^VGA/) {
			$sortedChannels[9] = $outputName;
		} else {
			$sortedChannels[$index] = $outputName;
			$index++;
		}
	}
	return @sortedChannels;
}

1;
