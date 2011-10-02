#!/usr/bin/perl
# Copyright (c) 2004 SuSE AG Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2004
# libsax script: update /boot/grub/menu.lst in order to allow fbdev setup
# --
# Status: Development
#
use strict;

#---[ setupBootLoaderVGA ]----#
sub setupBootLoaderVGA {
#----------------------------------------------
# search default entry and change the vga=0x..
# parmater of this entry
#
	my $mode  = shift;
	my $menu  = "/boot/grub/menu.lst";
	my $next  = "$menu.sax";
	my $deflt = 0;
	my $count = -1;
	my @data  = ();
	my $stream;
	my $perm;
	if ( ! -f $menu ) {
		print "*** file checked failed: $menu: $!\n";
		return undef;
	}
	if (! open (FD,"<",$menu)) {
		print "*** failed open file: $menu: $!\n";
		return undef;
	}
	$mode = sprintf ("%0x%s",$mode);
	if ($mode == 0) {
		print "*** invalid mode, value equals zero\n";
		return undef;
	}
	$perm = (stat FD)[2] & 07777;
	@data = <FD>; close FD;
	foreach my $line (@data) {
		next if ($line =~ /^[\s\t]*#/);
		if ($line =~ /default\s+(\d)/) {
			$deflt = $1;
		}
	}
	foreach my $line (@data) {
		next if ($line =~ /^[\s\t]*#/);
		if ($line =~ /^\s*title\s/i) {
			$count++;
		}
		if ($line=~ /^\s*kernel\s/i && $count == $deflt) {
			$line=~s/\bvga=(normal|ask|0x[0-9a-f]{1,4}|\d{1,5})\b/vga=0x$mode/i;
			last;
		}
	}
	$stream = join ("",@data);
	if (! open (FD,">",$next)) {
		print "failed open file: $next: $!\n";
		return undef;
	}
	print FD $stream; close FD;
	chmod ($perm,$next);
	qx (mv $next $menu);
}

if (! setupBootLoaderVGA ($ARGV[0])) {
	exit 1
}
exit 0
