#!/usr/bin/perl
# Copyright (c) 2005 SUSE LINUX Products GmbH, Germany. All rights reserved.
#
# Author: Marcus Schäfer <sax@suse.de>, 2005
# libsax script: apply profile on specific device number
# --
#
# createPRO 
#   -p <ProfileFile> -d <DeviceNumber>
#
# --
#
use strict;
#==========================================
# Module path
#------------------------------------------
use lib '/usr/share/sax/modules';

#==========================================
# Includes
#------------------------------------------
use Storable;
use Getopt::Long;
use SPPParse;

#==========================================
# Globals
#------------------------------------------
my $Profile;
my $Device;

#==========================================
# init
#------------------------------------------
sub init {
	# ...
	# This function is called to evaluate the options and
	# import the SaX registry for accessing the init() stage
	# configuration information.
	# ---
	my %import = {};
	my $config = "/var/cache/sax/files/config";
	my $result = GetOptions(
		"profile|p=s" => \$Profile,
		"device|d=i"  => \$Device,
		"<>"          => \&usage
	);
	if ( $result != 1 ) {
		usage();
	}
	if (-s $config) {
		my $hashref = retrieve($config);
		if (defined $hashref) {
			%import = %{$hashref};
		}
	}
	return %import;
}

#==========================================
# ImportProfile
#------------------------------------------
sub ImportProfile {
	# ...
	# This function will import the given profile and return
	# the modified data dictionary 
	# ---
	my %info   = %{$_[0]};
	my %import = {};
	if (! defined $Device) {
		$Device = 0;
	}
	prepareProfile ( $Profile,$Device );
	addProfileData ("Screen->$Device->Device=Device[$Device]");
	addProfileData ("Screen->$Device->Monitor=Monitor[$Device]");
	addProfileData ("Screen->$Device->Identifier=Screen[$Device]");
	addProfileData ("Monitor->$Device->Identifier=Monitor[$Device]");
	%import = includeProfile ( \%import,\%info );
	my $max = 0;
	foreach my $id (keys %{$import{Device}}) {
		if ($id > $max) {
			$max = $id ;
		}
	}
	$Device = $max + 1;
	%import = HUpdateServerLayout (
		\%import,$Device
	);
	return %import;
}

#==========================================
# main
#------------------------------------------
sub main {
	# ...
	# Import the profile and create a temporary profile
	# data cache. The cache will be read in by isax and the
	# output is evaluated with the raw profile line
	# ---
	#==========================================
	# import and apply profile
	#------------------------------------------
	my %import = {};
	%import = init();
	%import = ImportProfile ( \%import );

	#==========================================
	# save profile cache
	#------------------------------------------
	my $datafile = "/var/lib/sax/createPRO.cache";
	unlink ($datafile);
	if (! store(\%import,$datafile)) {
		die "createPRO: could not create tree: $datafile\n";
	}
	#==========================================
	# call isax on profile cache
	#------------------------------------------
	my $isax = "/usr/sbin/isax -b $datafile -l ";
	my $card     = qx ($isax Card);
	my $desktop  = qx ($isax Desktop);
	my $keyboard = qx ($isax Keyboard);
	my $mouse    = qx ($isax Mouse);
	my $path     = qx ($isax Path);
	my $layout   = qx ($isax Layout);

	#==========================================
	# print profile cache
	#------------------------------------------
	printProfile ($card     , "Card"    );
	printProfile ($desktop  , "Desktop" );
	printProfile ($keyboard , "Keyboard");
	printProfile ($mouse    , "Mouse"   );
	printProfile ($path     , "Path"    );
	printProfile ($layout   , "Layout"  );

	#==========================================
	# remove profile cache
	#------------------------------------------
	unlink ($datafile);
}

#==========================================
# printProfile
#------------------------------------------
sub printProfile {
	# ...
	# print profile information to stdout and take care
	# about empty entries
	# ---
	my @data = split (/\n/,$_[0]);
	my $item = $_[1];
	foreach my $line (@data) {
	if ($line =~ /^\d+ : .+ : (.+)/) {
		my $value = $1;
		$value =~ s/ +//g;
		if ($value ne "") {
			print "$line:$item\n";
		}
	}
	}
}

#==========================================
# usage
#------------------------------------------
sub usage {
	# ...
	# if you need help :-)
	# ---
	exit (0);   
}

main();
