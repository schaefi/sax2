#!/usr/bin/perl
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
# sysp script: transform value of KEYTABLE variable
# --
# This script is called via the KEYTABLE reading
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;
use FileHandle;

#----[ CheckMap ]----------#
sub CheckMap {
#---------------------------------------------------
# This function checks if the argument
# contains .map.gz or not and add it if needed
#
	my $map = $_[0];
	my $kbd = "/usr/share/sax/sysp/maps/Keyboard.map";

	if ($map eq "") {
		$map = "us.map.gz";
	}

	$map =~ s/#.*//;
	$map =~ s/^ +//;
	$map =~ s/ +$//;
	if ($map =~ /(.*)\.map\.gz.*/) {
		$map = $1;
	}

	my $ok = 0;
	my $fh = new FileHandle;
	if ($fh->open("< $kbd")) {
	while (my $line = <$fh>) {
		if ($line =~ /^\Q$map\E.*/) {
			$ok = 1; last;
		}		
	}
	}
	$fh->close;
	if ($ok) {
		print "$map\n";
	} else {
		print "us\n";
	}
}

# check for map suffix...
# ------------------------
CheckMap($ARGV[0]);

