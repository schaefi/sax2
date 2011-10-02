#!/usr/bin/perl
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2005
# sysp script: lookup driver statement in profiles
# --
# This script is called via the mouse detection
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;

sub getDriver {
	my $file = $_[0];
	if (! open (FD,"$file")) {
		return "mouse";
	}
	while (<FD>) {
	if ($_ =~ /Driver.*=(.*)/) {
		my $driver = $1;
		$driver =~ s/ +//g;
		return $driver;
	}
	}
	return "mouse";
}

my $driver = getDriver ($ARGV[0]);
print "$driver\n";
exit (0);
