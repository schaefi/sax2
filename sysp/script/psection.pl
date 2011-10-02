#!/usr/bin/perl
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
# sysp script: get number of device sections in the given profile
# --
# This script is called via the SERVER import
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;

#---[ CountSections ]-----#
sub CountSections {
#------------------------------------------------
# read the given profile file and count the
# number of device sections which may be added with
# this profile
#
	my $file = $_[0];
	if (! open (FD,"$file")) {
		return (0);
	}
	my %addsection;
	while (<FD>) {
	if ($_ =~ /.*->.*(\[X\+[0-9]+\]).*->.*/) {
		$addsection{$1} = $1;
	}
	}
	return (
		keys %addsection
	);
}

my $count = CountSections ( 
	$ARGV[0] 
);
print "$count\n";
exit ( $count );
