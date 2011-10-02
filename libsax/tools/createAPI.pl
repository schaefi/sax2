#!/usr/bin/perl
# Copyright (c) 2004 SuSE AG Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2004
# libsax script: create xorg.conf configuration file using ISaX
# --
# Status: Development
#
use strict;
use FileHandle;
use SaX;

#=====================================
# Globals 
#-------------------------------------
my %spec;

#---[ init ]----#
sub init {
#----------------------------------------------
# init global specifications in %spec hash
#
	$spec{CDir}  = "/var/lib/sax/";
	$spec{ISaX}  = "/sbin/isax";
}

#---[ createXOrgConfig ]-----#
sub createXOrgConfig {
#----------------------------------------------
# create configuration file using ISaX
#
	if (-f "$spec{CDir}/keyboard") {
		qx (cat $spec{CDir}/keyboard | grep '^0' > $spec{CDir}/keyboard.new);
		qx (cat $spec{CDir}/keyboard | grep -v '^0' >> $spec{CDir}/input);
		qx (mv  $spec{CDir}/keyboard.new $spec{CDir}/keyboard);
	}
	my $patchBIOS = 0;
	my $BIOSData  = "ok";
	my @configFile = (
		"card",
		"desktop",
		"input",
		"keyboard",
		"layout",
		"path",
		"extensions"
	);
	my $api = new FileHandle "> $spec{CDir}/apidata";
	if (! defined $api) {
		exit (3);
	}
	foreach my $file (@configFile) {
		my $handle = new FileHandle;
		if ($handle -> open("< $spec{CDir}/$file")) {
			SWITCH: for ($file) {
			/^card/     && do {
				print $api "Card \{\n";
				last SWITCH;
			};
			/^desktop/  && do {
				print $api "Desktop \{\n";
				last SWITCH;
			};
			/^input/      && do {
				print $api "Mouse \{\n";
				last SWITCH;
			};
			/^keyboard/   && do {
				print $api "Keyboard \{\n";
				last SWITCH;
			};
			/^layout/     && do {
				print $api "Layout \{\n";
				last SWITCH;
			};
			/^path/       && do {
				print $api "Path \{\n";
				last SWITCH;
			};
			/^extensions/ && do {
				print $api "Extensions \{\n";
				last SWITCH;
			};
			}
			while (<$handle>) {
				chomp ($_); my @list = split (/ : /,$_);
				my $line = sprintf (" %s %-20s =     %s\n",
					$list[0],$list[1],$list[2]
				);
				if ($list[2] =~ /^i810 *$/) {
					$patchBIOS=1;
				}
				if ($list[1] =~ /^Modes:16 *$/) {
					my @reslist = split (/,/,$list[2]);
					$BIOSData = shift (@reslist);
				}
				print $api $line;
			}
			print $api "\}\n\n";
		}
		$handle->close;
	}
	$api->close();
	if ($patchBIOS == 1) {
		return $BIOSData;
	} else {
		return "ok";
	}
}

#=====================================
# getIntelPatchMap
#-------------------------------------
sub getIntelPatchMap {
	my $resx = shift;
	my $resy = shift;
	my %result;
	if (! open (FD,"/usr/share/sax/sysp/maps/IntelPatch.map")) {
		return undef;
	}
	foreach my $line (<FD>) {
		if ($line =~ /^#/) {
			next;
		}
		if ($line =~ /(.*):(.*):(.*)/) {
			my $code = $1;
			my $args = $2;
			my $prog = $3;
			$code =~ s/^ +//; $code =~ s/ +$//;
			$args =~ s/^ +//; $args =~ s/ +$//;
			$prog =~ s/^ +//; $prog =~ s/ +$//;
			if ((defined $resx) && (defined $resy)) {
				$args =~ s/\%x/$resx/;
				$args =~ s/\%y/$resy/;
			}
			$result{$code}{$prog} = $args;
		}
	}
	close  FD;
	return %result;
}

#=====================================
# getIntelPatchCode
#-------------------------------------
sub getIntelPatchCode {
	my %result = ();
	my $xstuff = new SaX::SaXImportSysp ($SaX::SYSP_DESKTOP);
	my $server = new SaX::SaXImportSysp ($SaX::SYSP_CARD);
	$xstuff -> doImport();
	$server -> doImport();
	for (my $i=0;$i<$server -> getCount();$i++) {
		my $code = "0x0";
		$xstuff -> setID ($i);
		$server -> setID ($i);
		my $ddc = $xstuff -> getItem ("DDC");
		my $vid = $server -> getItem ("VID");
		my $did = $server -> getItem ("DID");
		my $svd = $server -> getItem ("SUB-VID");
		my $sdd = $server -> getItem ("SUB-DID");
		if ($ddc eq "<undefined>") {
			$ddc = "*******";
		}
		$code = "$vid$did$svd$sdd$ddc";
		$code =~ s/0x//g;
		$code = "0x".$code;
		$result{$code}{ddc} = $ddc;
		$result{$code}{vid} = $vid;
		$result{$code}{did} = $did;
		$result{$code}{svd} = $svd;
		$result{$code}{sdd} = $sdd;
	}
	return %result;
}

#=====================================
# Run... 
#-------------------------------------
init();
my $patchBIOS = createXOrgConfig();
if ($patchBIOS ne "ok") {
	#=====================================
	# i810 device: check for patching bios
	#-------------------------------------
	my @xy = split (/x/,$patchBIOS);
	my %patch = getIntelPatchMap  ($xy[0],$xy[1]);
	my %pcode = getIntelPatchCode ();
	foreach my $icode (keys %pcode) {
	foreach my $code (keys %patch) {
		if ($code !~ /0x(....)(....)(....)(....)(.......)/) {
			print "SaX: Invalid Intel patch code: $code\n";
			next;
		}
		my $vid = $1;
		my $did = $2;
		my $svd = $3;
		my $sdd = $4;
		my $ddc = $5;
		if ($ddc eq '*******') {
			$ddc = $pcode{$code}{ddc};
		}
		if ($vid eq '****') {
			$vid = $pcode{$code}{vid};
		}
		if ($did eq '****') {
			$did = $pcode{$code}{did};
		}
		if ($svd eq '****') {
			$svd = $pcode{$code}{svd};
		}
		if ($sdd eq '****') {
			$sdd = $pcode{$code}{sdd};
		}
		$code = "$vid$did$svd$sdd$ddc";
		$code =~ s/0x//g;
		$code = "0x".$code;
		if ($code eq $icode) {
			next
		}
		foreach my $prog (keys %{$patch{$code}}) {
		if (-f $prog) {
			my $args = $patch{$code}{$prog};
			print "SaX: Patching BIOS: $prog $args\n";
			my $result = qx ($prog $args 2>&1);
			my $key  = "VIDEOBIOS_PARAMETERS";
			my $file = "/etc/sysconfig/videobios";
			my $tmpf = "$file.tmp";
			if (-f $file) {
				qx (cat $file | sed -e s!$key=.*!$key=\\""$args"\\"! > $tmpf);
				qx (mv $tmpf $file);
				$key = "VIDEOBIOS_PATCH";
				qx (cat $file | sed -e s!$key=.*!$key=\\""yes"\\"! > $tmpf);
				qx (mv $tmpf $file);
			}
		}
		}
	}
	}
}
