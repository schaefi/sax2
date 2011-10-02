#!/usr/bin/perl

use lib '/usr/share/sax/profile';

use strict;
use Storable;
use Profile;

#====================================
# Functions...
#------------------------------------
sub getProfile {
	my $config = "/var/cache/sax/files/config";
	my $card   = 0;
	my $hashref = retrieve($config);
	if (! defined $hashref) {
		die "MergedFB: can not load sax registry";
	}
	my %dialog = %{$hashref};
	my $driver = $dialog{Device}{$card}{Driver};
	my $dir = "/usr/share/sax/profile";
	SWITCH: for ($driver) {
		/^i810|i915/ && do {
			return "$dir/Intel_DualHead_DriverOptions";
		};
		/^intel/     && do {
			return "$dir/IntelNext_DualHead_DriverOptions";
		};
		/^nvidia/    && do {
			return "$dir/NVidia_DualHead_DriverOptions";
		};
		/^radeonhd/    && do {
			return "$dir/RadeonHD_DualHead_DriverOptions";
		};
		/^radeon/    && do {
			return "$dir/Radeon_DualHead_DriverOptions";
		};
		/^fglrx/     && do {
			return "$dir/FGLRX_DualHead_DriverOptions";
		};
		/^mga/       && do {
			return "$dir/Matrox_DualHead_DriverOptions";
		};
		/^sis/       && do {
			return "$dir/SiS_DualHead_DriverOptions";
		};
		die "MergedFB: no DualHead profile available";
	}
}

#====================================
# Init profile script
#------------------------------------
my $profile = ProfileInitScript();

#====================================
# Do the profile adaptions...
#------------------------------------
my $used = getProfile();
print STDERR "MergedFB: selected profile: $used\n";
if ( -f "$used.pl" ) {
	print STDERR "MergedFB: calling profile script: $used.pl\n";
	qx ($used.pl);
	my $base = qx (basename $used);
	chomp $base;
	$used="/var/lib/sax/$base.tmp";
}
qx (cp $used $profile);
