#!/usr/bin/perl
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
# sysp script: remove FontPath entries from X11 log
# --
# This script is called via the cfg stuff to cut the 
# huge FontPath lines to prevent us from Lex/YaCC problems
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;

#----[ PrepareLog ]-----#
sub PrepareLog {
#---------------------------------------------
# this function will remove all the FontPath
# settings from the given file
#
	my $log = $_[0];
	my $new = $log.$$;

	if (! -f $log) {
		return;
	}
	unlink($new);
	qx(cat $log | grep -v FontPath > $new);
	if (-f $new) { 
		qx(mv  $new $log);
	}
}

PrepareLog ($ARGV[0]);
