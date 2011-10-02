#!/usr/bin/perl
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
# sysp script: get machine type
# --
# This script is called via the 3D stuff to find 
# a package in the RPM database
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;

#----[ CheckPackage ]----------#
sub CheckPackage {
#---------------------------------------------------
# This function checks via rpm if a package is
# installed or not.
#
	my $package = $_[0];

	if ($package ne "<none>") {
		qx(/bin/rpm -q $package 2>/dev/null >/dev/null);
		if ($? == 0) { print "yes\n" ; exit(1); }
	}
	print "no\n";
	exit(0);
}

# check for package...
# ---------------------
CheckPackage($ARGV[0]);

