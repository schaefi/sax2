# /.../
# Copyright (c) 2000 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
#
# configuration level 1 auto detect code
# perl package providing subroutines to handle secure
# tmp directories
#
# CVS ID:
# --------
# Status: Up-to-date
#
package Secure;

my $TmpDir;

#---[ CreateSecureDir ]-----#
sub CreateSecureDir {
#----------------------------------------------
# this function create a secure tmp directory
# and return the name of the directory
#
	my $prefix = $_[1];

	if ($prefix eq "") {
		$prefix = "sysp";
	}

	$TmpDir = "$prefix-$$";
	qx (rm -rf /tmp/$TmpDir);
	$result = mkdir("/tmp/$TmpDir",0700);
	if ($result == 0) {
		print "secure: could not create tmp dir... abort\n";
		exit(1);
	}
	return($TmpDir);
}


#---[ GetTmpDir ]-----#
sub GetTmpDir {
#----------------------------------------------
# returning tmp dir name
#
	return($TmpDir);
}


#---[ CleanTmp ]----#
sub CleanTmp {
#----------------------------------------------
# this function send a signal to the process
# itself and forces removement of the tmp dir
#
	kill (15,$$);
}

#---[ RemoveDir ]-----#
sub RemoveDir {
#----------------------------------------------
# this function removes the secure dir and
# all its contents
#
	qx (rm -rf /tmp/$TmpDir);
}

1;

