#!/usr/bin/perl
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
# sysp script: get machine type
# --
# This script is called via the server stuff to kill 
# the dots if they are running
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;

#----[ KillDots ]-----#
sub KillDots {
#------------------------------------
# kill the dots if exist...
#
	qx(killall dots 2>/dev/null);
	print "done\n";
}

KillDots();

