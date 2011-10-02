#!/usr/bin/perl
# Copyright (c) 2005 SuSE GmbH Nuernberg, Germany.  All rights reserved.
# Author: Marcus Schaefer <sax@suse.de>, 2005
#
# libsax script: createTST.pl
# perl script to start X11 and the tuning tool
# This script is called from within the testConfiguration()
# method of the SaXConfig class
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;

use Getopt::Long;
use FileHandle;
use POSIX;
use English;

#=========================
# Globals...
#-------------------------
my $choose = 1;
my $server;
my %var;

main();
#======================================================
# this script will start the X-Server and create
# the environment to be able to run XFine on it. This
# includes thestart of the XIdle daemon and the helper
# boundary around the desktop
#-------------------------------------------------------
#
#--[ main ]-----#
sub main {
#-----------------------------------
# main routine to start server and
# tuning tool
#
	#========================================
	# get/validate options
	#----------------------------------------
	my $Display;
	my $result = GetOptions(
		"display|d=s" => \$Display,
		"help|h"      => \&usage,
		"<>"          => \&usage
	);
	if ( $result != 1 ) {
		usage();
	}
	if ($Display eq "") {
		$Display = ":2.0";
	}
	if ($UID != 0) {
		die "createTST: only root can do this";
	}
	#========================================
	# set specifications...
	#----------------------------------------
	$var{X}             = "/usr/sbin/xw";
	$var{Xidle}         = "/usr/sbin/xapi";
	$var{XFine}         = "/usr/sbin/xfine";
	$var{Xpid}          = "/var/log/SaX.pid";
	$var{SaXLog}        = "/var/log/SaX.log";
	$var{LastLog}       = "/var/lib/sax/lastlog";
	$var{ConfigFile}    = "/var/lib/sax/xorg.conf";
	$var{FirstConfig}   = "/var/lib/sax/xorg.conf.first";

	#========================================
	# set signal handler...
	#----------------------------------------
	$SIG{TERM} = "Quit";
	$SIG{INT}  = "Quit";
	# /.../
	# the initiial X-Terminal on a new server startup is
	# saved at /var/log/xvt. This terminal plus 1 is the
	# test terminal for the new X-Server.
	# -----------------------------------
	my $tty = GetXTerminal(); $tty++;

	#========================================
	# run server at vt $tty...
	#----------------------------------------
	unlink($var{LastLog});
	my $XLoader = "$var{X} $var{LastLog}:xfine -xf86config $var{ConfigFile}";
	my $XFlags  = "-allowMouseOpenFail -br -verbose 2 +accessx vt$tty";
	system("$XLoader $XFlags $Display >$var{Xpid} 2>/dev/null");
	my $exitStatus = $? >> 8;
	if ($exitStatus ne "0") {
		$tty--; qx (chvt $tty);
		exit (1);
	}
	if (! open  (FD,"$var{Xpid}")) { 
		die "createTST: could not open file: $var{Xpid}";
	}
	while (<FD>) { $server = $_; } 
	close (FD);

	unlink ($var{Xpid});
	logger ($var{ConfigFile},$var{LastLog},$var{SaXLog});
	$server =~ s/\n//g;

	#========================================
	# switch to the test server terminal
	#----------------------------------------
	my $proc = qx (LANG=POSIX ps --no-headers $server);
	if ($proc eq "") {
		$tty--; qx (chvt $tty);
		exit (1);
	}
	qx (chvt $tty);

	#========================================
	# start XFine2 on the server...
	#----------------------------------------
	my $xfinePid = fork();
	if (! defined $xfinePid) {
		die "createTST: could not fork...abort";
	} elsif ($xfinePid) {
		$var{xfinePid} = $xfinePid;
		waitpid ($xfinePid,0);
		$choose = $? >> 8;
	} else {
		exec ("$var{XFine} -display $Display >/dev/null 2>&1");
	}

	#========================================
	# cleaning X-Server clients...
	#----------------------------------------
	Quit();
};

#----[ Quit ]------#
sub Quit {
#-----------------------------------------------------
# exit all running X11 clients and quit the
# X-Server than...
#
	Kill ($server);
	print "$choose\n";
	exit ($choose);
}

#-----[ logger ]------#
sub logger {
#---------------------------------------
# append logging data to file...
#
	my $datafile   = $_[0];
	my $configfile = $_[1];
	my $file       = $_[2];
	my $date   = qx (
		LANG=POSIX /bin/date "+%d-%h %H:%M:%S"
	);
	chomp ($date);

	my @data;
	my $diff;

	# /.../
	# if FirstConfig does not exist:
	# - create it and save file contents as first log
	# - otherwhise create a diff and save the diff
	# --------------------------------------------
	if (! -f $var{FirstConfig}) {
		qx (cp $datafile $var{FirstConfig});
		push (@data,"Configuration: [FIRST SETUP]...\n");
		open (FD, "$datafile");
		while(<FD>) {
			push (@data,$_);
		}
		close(FD);
	} else {
		push (@data,"Configuration: [NEXT SETUP DIFF]...\n");
		$diff = qx (diff -u $var{FirstConfig} $datafile);
		if ($diff ne "") {
		foreach my $line (split("\n+",$diff)) {
			push (@data,$line);
		}
		} else {
			push (@data,"no differences\n");
		}
	}
	open (FD,">>$file");
	my $topic = shift (@data);
	chomp ($topic);
	print FD "$date <T> $topic [\n";
	foreach my $line (@data) {
		chomp ($line);
		print FD "\t$line\n";
	}
	print FD "]\n";

	# /.../
	# open log and save it to list...
	# ---------------------------------
	open (IO, "$configfile");
	print FD "$date <T> Configuration: [LOG]... [";
	while(<IO>) {
		chomp ($_);
		print FD "\t$_\n";
	}
	print FD "]\n";
	close (IO);
	close (FD);
}

#---[ Kill ]-------#
sub Kill {
#--------------------------------------
# my own kill function to make sure
# the process is really killed
#
	my $pid = $_[0];
	my $cnt;

	if ($pid eq "") {
		return;
	}
	kill(15,$pid);
}

#----[ GetXTerminal ]---------#
sub GetXTerminal {
#----------------------------------------------------
# this function read the /var/log/xvt file and 
# return the number of the X terminal SaX2 was 
# running on
#
	my $file = "/var/log/xvt";
	my $nr;

	open (FD,$file);
	while(<FD>) {
	if ($_ =~ /TTyX:(.*)/) {
		$nr = $1;
	}
	}
	close(FD);
	if ($nr eq "") {
		$nr = 7;
	}
	return($nr);
}

#---[ usage ]------#
sub usage {
#-----------------------
# if you need help :-)
#
	print "Linux SaX2 (libsax) (2005-01-19)\n";
	print "(C) Copyright 2005 SuSE GmbH\n";
	print "\n";

	print "usage: createTST [ options ]\n";
	print "options:\n";
	print "[ -d | --display < string > ]\n";
	print "   set display string for startup\n";
	print "--\n";
	exit(0);
}
