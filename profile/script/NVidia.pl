#!/usr/bin/perl

use lib '/usr/share/sax/profile';

use strict;
use Profile;

#====================================
# Init profile script
#------------------------------------
my $profile = ProfileInitScript();

#====================================
# Check if profile is valid
#------------------------------------
if (ProfileIsXOrgVendor ("nvidia")) {
	open (FD,">",$profile) ||
		die "NVidia: Can't open $profile: $!";
	print FD "Device -> [X] -> Driver = nv\n";
	close FD;
}
