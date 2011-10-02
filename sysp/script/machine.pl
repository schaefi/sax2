#!/usr/bin/perl
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
# sysp script: get machine type
# --
# This script is called via the PPC block within 
# keyboard.c
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;

my $machine = qx(awk '/^machine/ { print $3 }' < /proc/cpuinfo);
SWITCH: for ($machine) {
	/Apus/i                                   && do {
	# nothing to set for...
	# ----------------------
	last SWITCH; 
	};
	/Power Mac|iMac|PowerMac|PowerBook/i      && do { 
	# macintosh protocol...
	# ----------------------
	print "macintosh\n";
	last SWITCH;
	};
	/^CHRP|PReP/i                             && do { 
	# pc104 protocol...
	# ------------------------
	print "pc104\n";
	last SWITCH; 
	};
	# any other ( default )...
	# -------------------------
	print "pc105\n";
}

