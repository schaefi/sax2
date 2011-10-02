# /.../
# Copyright (c) 2002 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2002
#
# CheckServer.pm configuration level 1
# provide functions to search for X-Server process ID`s
# and check if those server are accessable from the current
# user
#
# CVS ID:
# --------
# Status: Up-to-date
#

#---[ GetDisplay ]-----#
sub GetDisplay {
#----------------------------------------------------
# this function get the display number according
# to the given pidlist. we will check for access
# to the server pid too
#
	my @pids   = @_;
	my $Corner = pop (@pids);
	my @display;
	my $dpy;

	# /.../
	# Check if we come from remote. If yes we assume
	# X11 forwarding and check for the DISPLAY variable
	# ---
	my @displayStatus;
	system ("/usr/sbin/whereiam");
	if ($? >> 8 == 1) {
	if (defined $ENV{DISPLAY}) {
		$displayStatus[0] = $ENV{DISPLAY};
		$displayStatus[1] = "grant";
		return @displayStatus;
	}		
	}
	# /.../
	# check on which display number the X-Server
	# process is running on
	# ---
	foreach my $pid (@pids) {
	my $file = "/proc/$pid/fd/0";
	if (-l $file) {
		my $log = readlink("/proc/$pid/fd/0");
		if (-f $log) {
			# /.../
			# descriptor 0 is a link pointing to the log file
			# check for the display number in the log file
			# name first
			# ---
			if ($log =~ /[XFree86|Xorg]\.(.*)\.log/) {
				my @list = split(/\./,$1);
				$list[0] =~ s/://;
				push (@display,$list[0]);
			}
		} else {
			# /.../
			# descriptor 0 is not a file so we are checking
			# the socket number for the server. socket - 6000
			# results in the display number used for this
			# connection
			# ---
			my $log = qx(socklist | grep $pid | tail -n 1);
			my @solist  = split (/ +/,$log);
			if (defined $solist[1]) {
				push (@display,$solist[1] - 6000);
			} else {
				# /.../
				# We couldn't find a TCP socket associated with
				# the given pid. Assuming descriptor 0 is a socket but
				# not a TCP socket... trying to check cmdline string
				# ---
				if (open (FD,"</proc/$pid/cmdline")) {
					my $cmdline = <FD>; close FD;
					if ($cmdline =~ /:(\d+)/) {
						push (@display,$1);
					}
				}
			}
		}
	}
	}
	# /.../
	# check which display we are allowed 
	# to access
	# ---
	sub numeric { $a <=> $b; }
	foreach $dpy (sort numeric @display) {
	my $status = qx($Corner -t x -d :$dpy 2>/dev/null);
	$status =~ s/\n//g;
	if ($status eq ":-)") {
		$displayStatus[0] = ":$dpy.0";
		$displayStatus[1] = "grant";
		return @displayStatus;
	}
	}
	$dpy++;
	$displayStatus[0] = ":$dpy.0";
	$displayStatus[1] = "denied";
	return @displayStatus;
}

#---[ GetVirtualTerminal ]------#
sub GetVirtualTerminal {
#----------------------------------------------------
# this function will probe for a free terminal to
# use. Start with vt7 and end with vt10
#
	my $fuser = "/bin/fuser";
	for ($i=7;$i<=20;$i++) {
	system ("$fuser /dev/tty$i 2>/dev/null >/dev/null");
	my $code = $? >> 8;
	if ($code != 0) {
		return "vt$i";
	}
	}
	print "SaX: can't find an unused tty... abort\n";
	die "init: can't find an unused tty";
}

#---[ GetPids ]-----#
sub GetPids {
#----------------------------------------------------
# this function search for a list of X Server PIDs. 
# We will look at FD [0] of the descriptor list
# to decide if this is an X11 server or not
#
	my $table = qx(ps axh | cut -c1-6);
	$table =~ s/ +//g;
	my @procs = split(/\n/,$table);
	@procs = reverse(@procs);
	my @pids;

	foreach (@procs) {
	my $file = "/proc/$_/exe";
	if (-l $file) {
		my $link = readlink ($file);
		if (($link =~ /XFree86|Xorg|Xgl/) || ($link =~ /XF.*_/)) {
			push(@pids,$_);
		}
	}
	}
	return @pids;
}

#---[ X11ForwardEnabled ]-----#
sub X11ForwardEnabled {
#----------------------------------------------------
# check if X11 forwarding is switched on in the
# terminal we want to start X
#
	my @displayStatus = ();
	my $corner = "/usr/sbin/corner";
	if (defined $ENV{DISPLAY}) {
		my $dpy = $ENV{DISPLAY};
		my $status = qx($corner -t x -d $dpy 2>&1);
		$status =~ s/\n//g;
		if ($status eq ":-)") {
			$displayStatus[0] = $dpy;
			$displayStatus[1] = "grant";
			return @displayStatus;
		}
	}
	$displayStatus[0] = ":0.0";
	$displayStatus[1] = "denied";
	return @displayStatus;
}

1;
