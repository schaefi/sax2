# /.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2001
#
# CreateSections.pm configuration level 2
# Create the DRI section of the xorg.conf
#
# CVS ID:
# --------
# Status: Up-to-date
#

#---[ CreateDRISection ]----#
sub CreateDRISection {
#------------------------------------------------
# create the DRI section as static section
# with fixed permissions
#
	my @result;
	push(@result,"Section \"DRI\"\n");
	push(@result,PrintLine("  Group","\"video\""));
	push(@result,PrintLine("  Mode","0660"));
	push(@result,"EndSection\n");
	return(@result);
}

1;
