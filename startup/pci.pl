#!/usr/bin/perl
# Copyright (c) 2000 SuSE GmbH Nuernberg, Germany.  All rights reserved.
# Author: Marcus Schaefer <sax@suse.de>, 2000
#
# Get the current PCI information, this part is called
# using sax2 -p
#
# CVS ID:
# --------
# Status: Up-to-date
#
use Getopt::Long;

#---[ lets start :-) ]----#
init(); main();

#----[ init ]-----#
sub init {
	# variable init...
	# ------------------
	$var{Sysp}     = "/usr/sbin/sysp";

	# option init...
	# ----------------
	undef($ProfileMode);

	$result = GetOptions(
		"profile|p"         => \$ProfileMode,
		"help|h"            => \&usage,
		"<>"                => \&usage
	);
	if ( $result != 1 ) {
		usage();
	}
}

#---[ main ]-----#
sub main {
#--------------------------
# do the job :-)
#
	my $data;

	if (defined $ProfileMode) {
		$data = qx($var{Sysp} -p -A no);
	} else {
		$data = qx($var{Sysp} -c);
	}
	print $data;
}

#---[ usage ]-----#
sub usage {
#-------------------------
# usage message
#
	print "usage: pci [ options ]\n";
	print "options:\n";
	print "[ -p | --profile ]\n";
	print "   print profile file for each card\n";
	exit(0);
}

