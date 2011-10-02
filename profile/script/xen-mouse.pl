#!/usr/bin/perl

use lib '/usr/share/sax/profile';

use strict;
use Profile;

#====================================
# Init profile script
#------------------------------------
my $profile = ProfileInitScript();

#====================================
# Get RealDevice from sysp
#------------------------------------
my $card   = ($ARGV[0] - 1) / 2;
my $device = ProfileGetRealMouseDevice ($card);

#====================================
# Adapt profile device
#------------------------------------
local $/;
open (FD,"<",$profile) ||
	die "xen-mouse: Can't open $profile: $!";
my $profileData = <FD>;
$profileData =~ s/\/dev\/input\/mice/$device/;
close FD;
open (FD,">",$profile) ||
	die "xen-mouse: Can't open $profile: $!";
print FD $profileData;
close FD;
