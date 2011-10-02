# /.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2001
#
# CreateSections.pm configuration level 2
# provide create functions which are using the var hash 
# to generate the single config sections
#
# CVS ID:
# --------
# Status: Up-to-date
#
use lib '/usr/share/sax/modules/create';

use device;
use dri;
use files;
use flags;
use header;
use input;
use layout;
use modes;
use module;
use monitor;
use screen;
use extensions;

$Version = "8.1";

#---[ PrintLine ]----#
sub PrintLine {
#---------------------------------------------
# print a formatted config line. If the
# value begins with a # made this entry an
# optional comment entry
#
	my $key   = $_[0];
	my $value = $_[1];
	my $line;

	if ($value =~ /^.*#.*/) {
		$value =~ s/#//;
		$value =~ s/^\" +/\"/;
		$line = sprintf("  #%-12s %s\n",$key,$value);
	} else {
		$line = sprintf("  %-12s %s\n",$key,$value);
	}
	return($line);
}

1;
