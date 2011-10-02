#!/usr/bin/perl
# Copyright (c) 2003 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2003
# sysp script: read vendor string from X11 driver modules
# --
# This script is called via the 3D scan
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;

#---[ vendorName ]----#
sub vendorName {
#-----------------------------------------------
# call strings and filter the given object data
# to check for one of the vendor names listed
# in @vendor
#
	my $driver = $_[0];
	my @vendor = (
		"X.Org Foundation",
		"The XFree86 Project",
		"NVIDIA Corporation",
		"FireGL - ATI Technologies Inc."
	);
	my $lib  = "lib";
	my $arch = qx (uname -i); chomp $arch;
	if (($arch eq "x86_64") || ($arch eq "ia64")) {
		$lib = "lib64";
	}
	# Initial driver location Xorg v6
	my $drvfile = "/usr/X11/$lib/modules/drivers/".$driver."_drv.*";
	if (glob("/usr/X11/$lib/modules/updates/drivers/".$driver."_drv.*")) {
		# There is an update dir for Xorg v6... preferred
		$drvfile = "/usr/X11/$lib/modules/updates/drivers/".$driver."_drv.*";
	} elsif (glob("/usr/$lib/xorg/modules/updates/drivers/".$driver."_drv.*")) {
		# There is an update dir for Xorg v7... preferred
		$drvfile = "/usr/$lib/xorg/modules/updates/drivers/".$driver."_drv.*";
	} elsif (glob("/usr/$lib/xorg/modules/drivers/".$driver."_drv.*")) {
		# There is a  driver dir for Xorg v7... preferred
		$drvfile = "/usr/$lib/xorg/modules/drivers/".$driver."_drv.*";
	} 
	foreach my $vname (@vendor) {
		my $code = system ( "grep -q -a \"$vname\" $drvfile 2>/dev/null" );
		if ($code == 0) {
			return $vname;
		}
	}
	return ($vendor[0]);
}

my $vendor = vendorName ( $ARGV[0] );
print "$vendor\n";
exit (0);
