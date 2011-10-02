#!/usr/bin/perl

use strict;
use Getopt::Long;

my %var;
my $file;
my $Complete;

#----[ init ]-----#
sub init {
#----------------------------------
# init some data structures
#
	my $result = GetOptions (
		"file|f=s"   => \$file,
		"complete|c" => \$Complete
	);
	$var{Identity} = $file;
}

#----[ IdentityToCard ]------#
sub IdentityToCard {
#---------------------------------------------
# this function will create the Cards file 
# format out of the contents of the Identity 
# file. We will use the following 
# translations:
#
# use only the FLAG=DEFAULT entries
# 
# NAME      -->  build "NAME:DEVICE {"
# DEVICE  
# SERVER    -->  build "Driver|3DDriver     = ..."
# OPT       -->  build "Option|3DOption     = ..."
# RAW       -->  build "RawData|3DRawData   = ..."
# FLAG      -->  build "Flag                = ..."
# EXT       -->  build "Extension           = ..."
# PROFILE   -->  build "Profile|3DProfile   = ..."
# SCRIPT3D  -->  build "Script3D            = ..."
# PACKAGE3D -->  build "Package3D           = ..."
# 
	my @result;
	my @ident;
	my @use;
 
	# open the Identity and save the file in a list
	# ---------------------------------------------
	open(FD,"$var{Identity}") || 
		die "could not open file: $var{Identity}";

	while (my $line = <FD>) {
		chomp($line);
		if (($line =~ /^#/) || ($line =~ /^$/)) { 
			next; 
		}
		push (@ident,$line);
	}
	close(FD);

	# get the information from the lines. The 3D lines
	# are used if they exist
	# ----------------------
	foreach my $line (@ident) {
	if ($line =~ /.*&FLAG=DEFAULT/) {

		$line  =~ /.*&VID=(.*)&DID=(.*)&SERVER.*/;
		my $vid   = $1;
		my $did   = $2;
		my $svid  = "";
		my $sdid  = "";
		if ($line =~ /.*&SUBVENDOR=(.*)&SUBDEVICE=(.*)/) {
			$svid  = $1;
			$sdid  = $2;
		}
		my $found  = 0;
		my $D3Line = "";

		foreach my $line (@ident) {
		if ($line =~ /.*&FLAG=3D/) {
			$line =~ /.*&VID=(.*)&DID=(.*)&SERVER.*/;
			my $idv  = $1;
			my $idd  = $2;
			my $idsv = "";
			my $idsd = "";
			if ($line =~ /.*&SUBVENDOR=(.*)&SUBDEVICE=(.*)/) {
				$idsv = $1;
				$idsd = $2;
			}
			if ($svid ne "") {
			if (
				($idv  eq $vid)   && ($idd  eq $did) && 
				($idsv eq $svid) && ($idsd eq $sdid)
			) {
				$D3Line = $line;
				$found = 1;
			}
			} else {
			if (($idv eq $vid) && ($idd eq $did)) {
				$D3Line = $line;
				$found = 1;
			}
			}
			if ($found == 1) {
				last;
			}
		}  
		}

		if ($found == 1) {
			push(@use,"$line\n$D3Line");
		} else {
			push (@use, $line);
		}
	}
	}

	# get the needed fields...
	# ---------------------------
	foreach my $line (@use) {
	if ($line =~ /NAME=(.*)&DEVICE=(.*)&VID.*/) {
		push(@result,"$1:$2 {\n");
		my @parts = split(/\n/,$line);
		if (defined ($parts[1])) {
			push(@result," Flag      = 3D\n");
		} else {
			push(@result," Flag      = DEFAULT\n");
		}
		SWITCH: for ($parts[0]) {
		# which module to use...
		# -----------------------
		/SERVER=(.*)&EXT/    && do {
		if ($1 ne "") {
			push(@result," Driver    = $1\n");
		}
		};

		# which device options...
		# ------------------------
		/OPT=(.*)&RAW/       && do {
		if ($1 ne "") {
			push(@result," Option    = $1\n");
		}
		};

		# which profile is defined...
		# ---------------------------
		/PROFILE=(.*)&SCR/   && do {
		if ($1 ne "") {
			push(@result," Profile   = $1\n");
		}
		};

		# raw information for device section...
		# --------------------------------------
		/RAW=(.*)&PROFILE/  && do {
		if ($1 ne "") {
			push(@result," RawData   = $1\n");
		}
		};
    
		# raw information for device section (end of line)...
		# -----------------------------------------------------
		/RAW=(.*)/  && do {
		if (($1 ne "") && ($1 !~ /.*&PROFILE.*/)) {
			push(@result," RawData =   $1\n");
		}
		};
		if (defined $parts[1]) {
		SWITCH: for ($parts[1]) {
		# which 3D server to use...
		/SERVER=(.*)&EXT/    && do {
		if ($1 ne "") {
			push(@result," 3DDriver  = $1\n");
		}
		};

		# which script to run...
		# ----------------------
		/SCRIPT3D=(.*)&PAC/  && do {
		if ($1 ne "") {
			push(@result," Script3D  = $1\n");
		}
		};

		# which package to install...
		# ---------------------------
		/PACKAGE3D=(.*)&FL/  && do {
		if ($1 ne "") {
			push(@result," Package3D = $1\n");
		}
		};

		# which device options...
		# ------------------------
		/OPT=(.*)&RAW/       && do {
		if ($1 ne "") {
			push(@result," 3DOption  = $1\n");
		}
		};

		# raw information for device section...
		# --------------------------------------
		/RAW=(.*)&PROFILE/  && do {
		if ($1 ne "") {
			push(@result," 3DRawData = $1\n");
		}
		};

		# raw information for device section (end of line)...
		# -----------------------------------------------------
		/RAW=(.*)/  && do {
		if (($1 ne "") && ($1 !~ /.*&PROFILE.*/)) {
			push(@result," 3DRawData = $1\n");
		}
		};

		# which extensions to use...
		# --------------------------
		/EXT=(.*)&OPT/       && do {
		if ($1 ne "") {
			push(@result," Extension = $1\n");
		}
		};

		# which profile is defined...
		# ---------------------------
		/PROFILE=(.*)&SCR/   && do {
		if ($1 ne "") {
			push(@result," 3DProfile = $1\n");
		}
		};

		# which Vendor/Device ID's...
		# ---------------------------
		/.*&VID=(.*)&DID=(.*)&SERVER.*/ && do {
			if (defined $Complete) {
				push(@result," VendorID  = $1\n");
				push(@result," DeviceID  = $2\n");
			}
		};

		# which SUB Vendor/Device ID's...
		# -------------------------------
		/.*&SUBVENDOR=(.*)&SUBDEVICE=(.*)/  && do {
			if (defined $Complete) {
				push(@result," SVendorID = $1\n");
				push(@result," SDeviceID = $2\n");
			}
		};
		}
		}
		}
		push(@result,"}\n\n");
	}
	}
	close (FD);
	return (@result);
}


#----[ main ]------#
sub main {
#-----------------------------------
# main function, create the Cards
# file...
#
	init();
	my %done   = {};
	my @result = IdentityToCard();
	my $vendor = "";
	my $name   = "";
	foreach (@result) {
		if ($_ =~ /(.*):(.*) \{/) {
			$vendor = $1;
			$name   = $2;
			next;
		}
		if ($_ =~ /\}/) {
			$vendor = "";
			$name   = "";
		}
		if ($vendor ne "") {
			$done{$vendor}{$name}{$_} = $_;
		}
	}
	foreach my $vendor (keys %done) {
	foreach my $name (keys %{$done{$vendor}}) {
		print "$vendor:$name {\n";
		foreach my $data (keys %{$done{$vendor}{$name}}) {
			print $data;
		}
		print "}\n\n";
	}
	}
}
main();
