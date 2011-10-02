#!/usr/bin/perl
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
# sysp script: check if given profile add new devices and return
# the number of devices detected
# --
# This script is called via the server stuff to check if
# a profile will make a single card to a multihead card. In this
# we will not ask for 3D
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;

sub profileCount {
	my $profile = $_[0];
	my $path    = "/usr/share/sax/profile";
	my $count   = 0;
	if (! open (FD,"$path/$profile")) {
		return (0);
	}
	while (<FD>) {
	if ($_ =~ /\[X\+(.*)\]/) {
		$count = $1;
	}
	}
	return ($count);
}

my $count = profileCount ($ARGV[0]);
print ("$count\n");
exit (
	$count
);
