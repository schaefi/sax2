#!/usr/bin/perl
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
# xc.pl configuration level 2 after sax startup level 1 (init)
#
# CVS ID:
# --------
# Status: Up-to-date
#
use lib   '/usr/share/sax/modules';
use strict;

use FileHandle;
use CreateSections;
use CheckServer;
use ParseConfig;
use Getopt::Long;
use Storable;
use XFree; 
use SaX;
use English;

#==========================================
# Globals
#------------------------------------------
my %var;                   # hold all the configuration parameters
my %spec;                  # hold specifications for xc.pl
my $EnableAutoConf;        # option variable: enable auto config
my $YaSTMode;              # called from YaST2
my $StartWithSystemConfig; # option start with system config
my $EnableXF86AutoMode;    # option variable: enable auto modeline
my $DialogToStartWith;     # dialog to start with (xapi option)
my $Display;               # option variable: set Display 
my $TmpDir;                # secure dir name
my %tune;                  # tune hash for changed mode timings
my $logHandle;             # log file handle
my $FullScreen;            # full screen mode
my $NoCheckPacs;           # do not check package requirements
my $NoIntro;               # prevent intro appearance

#==========================================
# Call main...
#------------------------------------------
main();

#==========================================
# main
#------------------------------------------
sub main {
	# ...
	# This function will open the configuration hash (registry)
	# created by init.pl and will create configuration suggestion
	# from this data. If there is no X-Server running start
	# my own X-Server otherwhise try to access the running one.
	# The last step is to run the GUI (xapi) on the server
	# ---
	#==========================================
	# Initialize xc options and spec hash
	#------------------------------------------
	init();

	#==========================================
	# Open configuration hash...
	#------------------------------------------
	if ((prepare()) == -1) {
		print "xc: level 2... abort\n";
		Exit(0);
	}

	#==========================================
	# Open LOG file handler...
	#------------------------------------------
	$logHandle = OpenLog();

	#==========================================
	# Create config suggestion
	#------------------------------------------
	my @part0;    # header
	my @part1;    # Files section
	my @part2;    # Module section
	my @part3;    # ServerFlags section
	my @part4;    # InputDevice sections
	my @part5;    # Monitor sections
	my @part6;    # Modes sections
	my @part7;    # Device section
	my @part8;    # Screen section
	my @part9;    # ServerLayout section
	my @part10;   # DRI section
	my @part11;   # Extensions section
	@part0 = CreateHeaderSection ();
	@part1 = CreateFilesSection  (\%var);
	@part2 = CreateModuleSection (\%var);
	@part3 = CreateServerFlagsSection (\%var);
	@part4 = CreateInputDeviceSection (\%var);
	if (defined $EnableXF86AutoMode) {
		@part5 = CreateMonitorSection (\%var,"yes");
	} else {
		@part5 = CreateMonitorSection (\%var,"no");
		@part6 = CreateModesSection (\%var,"no"); 
	}
	@part7  = CreateDeviceSection (\%var);
	@part8  = CreateScreenSection (\%var);        
	@part9  = CreateServerLayoutSection (\%var);  
	@part10 = CreateDRISection ();                
	@part11 = CreateExtensionsSection (\%var);

	#==========================================
	# Write preliminary config file
	#------------------------------------------
	open (HANDLE,">$spec{AutoConf}");
	print HANDLE @part0;  print HANDLE "\n";
	print HANDLE @part1;  print HANDLE "\n";
	print HANDLE @part2;  print HANDLE "\n";
	print HANDLE @part3;  print HANDLE "\n";
	print HANDLE @part4;  print HANDLE "\n";
	print HANDLE @part5;  print HANDLE "\n";
	print HANDLE @part6;  print HANDLE "\n";
	print HANDLE @part7;  print HANDLE "\n";
	print HANDLE @part8;  print HANDLE "\n";
	print HANDLE @part9;  print HANDLE "\n";
	print HANDLE @part10; print HANDLE "\n";
	print HANDLE @part11; print HANDLE "\n";
	close HANDLE;

	#==========================================
	# Write automatic config file and exit
	#------------------------------------------
	if (defined $EnableAutoConf) {
		# /.../
		# Auto configuration is enabled. This will save the auto
		# detected configuration file as final config file
		# ---
		my %section;
		my $import;
		my @importID = (
			$SaX::SAX_CARD,
			$SaX::SAX_DESKTOP,
			$SaX::SAX_POINTERS,
			$SaX::SAX_KEYBOARD,
			$SaX::SAX_LAYOUT,
			$SaX::SAX_PATH,
			$SaX::SAX_EXTENSIONS
		);
		my $config = new SaX::SaXConfig;
		foreach my $id (@importID) {
			$import = new SaX::SaXImport ( $id );
			$import->setSource ( $SaX::SAX_AUTO_PROBE );
			$import->doImport();
			$config->addImport ( $import );
			$section{$import->getSectionName()} = $import;
		}
		$config->setMode ($SaX::SAX_NEW);
		$config->createConfiguration();
		$config->commitConfiguration();
		print "SaX: Automatic configuration is done\n";
		CleanTmp();
		Exit(0);
	}

	#==========================================
	# Check display server status
	#------------------------------------------
	my $disp = ":0.0";
	my $haveServer = 0;
	my @displayStatus = ();
	my @xpid = GetPids();
	if (@xpid > 0) {
		#============================================
		# check on local system according to pid's
		#--------------------------------------------
		@displayStatus = GetDisplay (@xpid,$spec{Corner});
		$disp = $displayStatus[0];
		if ($displayStatus[1] eq "grant") {
			$haveServer = 1;
		}
	} else {
		#============================================
		# check for remote access, x11 forward
		#--------------------------------------------
		@displayStatus = X11ForwardEnabled();
		if ($displayStatus[1] eq "grant") {
			$disp = $displayStatus[0];
			$haveServer = 1;
		}
	}

	#==========================================
	# Apply log message about display status
	#------------------------------------------
	Logger ("Startup...",$logHandle);
	if ($haveServer) {
		print "SaX: X-Server: $displayStatus[0] -> $displayStatus[1]\n";
		Logger ("Startup on already running Server:
			$displayStatus[0] -> $displayStatus[1]",
			$logHandle
		);
	} else {
		Logger ("Startup on new Server: <$disp>",
			$logHandle
		);
		Logger ($spec{AutoConf},
			$logHandle
		);
	}

	#==========================================
	# Start new X-Server if needed
	#------------------------------------------
	$spec{haveServer} = $haveServer;
	if ($haveServer == 0) {
		my $vt = GetVirtualTerminal();
		my $XLoader = "$spec{X} $spec{Xmsg} -br -xf86config $spec{AutoConf}";
		my $XFlags  = "-allowMouseOpenFail -verbose 10 +accessx $vt";
		my $XDisp   = $disp;
		if ($disp =~ /(:.*)\./) {
			$XDisp = $1;
		}
		system("$XLoader $XFlags $XDisp >$spec{XpidFile}");
		if (! open(FD,"$spec{XpidFile}")) {
			die "xc: could not open file: $spec{XpidFile}";
		}
		while (<FD>) { 
			$spec{Xpid} = $_; 
		}
		close(FD);
		unlink($spec{XpidFile});

		#==========================================
		# save terminal settings to /var/log/xvt
		#------------------------------------------
		my $terminal = 7;
		my $tty = Getty();
		unlink($spec{XTerminal});
		open (FD,">$spec{XTerminal}");
		print FD "TTyX:$terminal\n";
		print FD "TTyT:$tty\n";
		close(FD);

		#==========================================
		# Check if the server starts up correctly
		#------------------------------------------
		if (CheckPID($spec{Xpid}) != 0) {
			#==========================================
			# check for missing packages
			#------------------------------------------
			my $install = "<none>";
			my $data = qx($spec{Sysp} -q 3d);
			if ($data =~ /Card3D0.*Install.*:(.*)\n.*/) {
				$install = $1;
				$install =~ s/ +//g;
			}
			if ($install ne "<none>") {
				print "xc: missing base package:\n";
				print "    please install the following package(s):\n";
				print "\n";
				print "    $install\n";   
				print "\n";
				print "xc: abort...\n";
			} else {
				#==========================================
				# Server startup failed... abort
				#------------------------------------------
				print "\n";
				print "xc: sorry could not start configuration server\n";
				print "xc: for details refer to the log file:\n";
				print "\n";
				print "    $spec{LogFile}\n";
				print "\n";
				print "xc: abort...\n";
			}
			Logger($spec{Xmsg},$logHandle);
			unlink($spec{Xmsg});
			Exit(0);
		}
	}
	#==========================================
	# prepare the options passed to xapi (GUI)
	#------------------------------------------
	my @parent = getParentName();
	my $apiopt = "--middle";

	#=======================================
	# Check if we are called from YaST2
	#---------------------------------------
	if (defined $YaSTMode) {
		$apiopt = "--yast";
	}
	#=======================================
	# Check if we are called from SaX2
	#---------------------------------------
	if (($haveServer) && ($parent[0] eq "sax")) {
		$apiopt = "";
	}
	#=======================================
	# In case of own server; check size
	#---------------------------------------
	if ($haveServer == 0) {
		$apiopt .= " --sizecheck";
	}
	#=======================================
	# Check for package check flag
	#--------------------------------------- 
	if (! defined $NoCheckPacs) {
		system ("DISPLAY=$disp $spec{Xapi} $apiopt --checkpacs");
	}
	#=======================================
	# Check for fullscreen mode
	#---------------------------------------
	if (defined $FullScreen) {
		$apiopt = $apiopt." --fullscreen";
	}
	#==========================================
	# Apply log message about server status
	#------------------------------------------
	if ($haveServer == 0) {
		my $glxinfo = qx (glxinfo -display $disp);
		Logger ($spec{Xmsg},$logHandle);
		Logger ("GLX Info:\n$glxinfo",$logHandle);
		unlink ($spec{Xmsg});
	}

	#=======================================
	# Show the intro message
	#---------------------------------------
	my $exit = 2;
	if (defined $NoIntro) {
		#=======================================
		# Don't want to see a intro message
		#---------------------------------------
		$exit = 1;
	} else {
		if ($haveServer == 0) {
			#=======================================
			# Call intro and save exit code
			#---------------------------------------
			system ("DISPLAY=$disp $spec{Xapi} $apiopt --info");
			$exit = $? >> 8;
			#=======================================
			# Get information about our own server
			#---------------------------------------
			my @xpid = GetPids();
			if (@xpid > 0) {
				@displayStatus = GetDisplay (@xpid,$spec{Corner});
				$disp = $displayStatus[0];
				if ($displayStatus[1] ne "grant") {
					$exit = 2;
				}
			} else {
				$exit = 2;
			}
		} else {
			#=======================================
			# There is already a server to access
			#---------------------------------------
			$exit = 1;
		}
	}
	#=======================================
	# Handle the exit code from the intro
	#---------------------------------------
	my $exitCode = 0;
	if ($exit == 1) {
		#=======================================
		# (1) Run the GUI (xapi)...
		#---------------------------------------
		if (open (OPT,">$spec{StartOptions}")) {
			print OPT $apiopt;
			close OPT;
		}
		if (defined $DialogToStartWith) {
			$apiopt = "$apiopt -O $DialogToStartWith";
		}
		if ($haveServer == 0) {
			if ((defined $StartWithSystemConfig) && (HeaderOK())) {
				print "SaX: importing current configuration...\n";
				system ("DISPLAY=$disp $spec{Xapi} $apiopt");
			} else {
				print "SaX: using cache data...\n";
				system ("DISPLAY=$disp $spec{Xapi} $apiopt --mode auto");
			}
		} else {
			if ((-f $spec{HWFlag}) || (! HeaderOK())) {
				print "SaX: using cache data...\n";
				system ("DISPLAY=$disp $spec{Xapi} $apiopt --mode auto");
			} else {
				print "SaX: importing current configuration...\n";
				system ("DISPLAY=$disp $spec{Xapi} $apiopt");
			}
		}
		$exitCode = $? >> 8;
	} elsif ($exit == 0) {
		#=======================================
		# (0) Save the configuration file
		#---------------------------------------
		# /.../
		# recreate Module Section because some modues not used
		# for initial configuration are needed for final 
		# configuration
		# ----
		@part2  = CreateModuleSection(\%var);
		open (HANDLE,">$spec{AutoConf}");
		print HANDLE @part0;  print HANDLE "\n";
		print HANDLE @part1;  print HANDLE "\n";
		print HANDLE @part2;  print HANDLE "\n";
		print HANDLE @part3;  print HANDLE "\n";
		print HANDLE @part4;  print HANDLE "\n";
		print HANDLE @part5;  print HANDLE "\n";
		print HANDLE @part6;  print HANDLE "\n";
		print HANDLE @part7;  print HANDLE "\n";
		print HANDLE @part8;  print HANDLE "\n";
		print HANDLE @part9;  print HANDLE "\n";
		print HANDLE @part10; print HANDLE "\n";
		close(HANDLE);

		LinkConfiguration();
		$exitCode = 1;
	}
	#=======================================
	# In case of own server; Kill it now
	#---------------------------------------
	if ($haveServer == 0) {
		Kill ($spec{Xpid});
	}
	#=======================================
	# Clean temp dir and exit
	#---------------------------------------
	CleanTmp();
	Exit ($exitCode);
}

#==========================================
# CreateSecureDir
#------------------------------------------
sub CreateSecureDir {
	# ...
	# this function create a secure tmp directory
	# and return the name of the directory
	# ---
	my $saxdir = "sax2-$$";
	my $result = mkdir("/tmp/$saxdir",0700);
	if ($result == 0) {
		print "xc: could not create tmp dir... abort\n";
		Exit(0);
	}
	return($saxdir);
}

#==========================================
# CleanTmp
#------------------------------------------
sub CleanTmp {
	# ...
	# this function send a signal to the process
	# itself and forces removement of the tmp dir
	# ---
	kill (15,$$);
}

#----[ init ]----#
sub init {
	# ...
	# init spec hash and test for root privileges
	# ---
	#==========================================
	# Create secure directory
	#------------------------------------------
	$TmpDir = CreateSecureDir();

	$SIG{HUP}   = "HandleTmpSignal";
	$SIG{INT}   = "HandleTmpSignal";
	$SIG{QUIT}  = "HandleTmpSignal";
	$SIG{ABRT}  = "HandleTmpSignal";
	$SIG{SEGV}  = "HandleTmpSignal";
	$SIG{PIPE}  = "HandleTmpSignal";
	$SIG{TERM}  = "HandleTmpSignal";

	$spec{Sysp}        = "/usr/sbin/sysp"; 
	$spec{DbmFile}     = "/var/cache/sax/files/config"; 
	$spec{HWFlag}      = "/var/cache/sax/files/hardware.chg";
	$spec{LogFile}     = "/var/log/SaX.log";
	$spec{XpidFile}    = "/var/log/SaX.pid";
	$spec{AutoConf}    = "/tmp/$TmpDir/xorg.conf";
	$spec{Xmsg}        = "/var/log/xlog";
	$spec{X}           = "/usr/sbin/xw";
	$spec{Xapi}        = "/usr/sbin/xapi";
	$spec{Intro}       = "/usr/share/sax/intro.pl";
	$spec{Corner}      = "/usr/sbin/corner";
	$spec{XTerminal}   = "/var/log/xvt";
	$spec{StartOptions}= "/var/cache/sax/files/xapi.opt";

	#==========================================
	# Get options...
	#------------------------------------------
	undef ($spec{Xpid});
	undef ($YaSTMode);
	undef ($EnableAutoConf);
	undef ($Display);
	undef ($FullScreen);
	undef ($NoCheckPacs);
	undef ($EnableXF86AutoMode);
	undef ($StartWithSystemConfig);
	undef ($DialogToStartWith);
	undef (%tune);

	my $result = GetOptions(
		"auto|a"         => \$EnableAutoConf,
		"yast|y"         => \$YaSTMode,
		"xmode|x"        => \$EnableXF86AutoMode,
		"sysconfig|s"    => \$StartWithSystemConfig,
		"fullscreen|f"   => \$FullScreen,
		"nocheckpacs|p"  => \$NoCheckPacs,
		"nointro|n"      => \$NoIntro,
		"dialog|O=s"     => \$DialogToStartWith,
		"help|h"         => \&usage,
		"<>"             => \&usage
	);
	if ( $result != 1 ) {
		usage();
	}
	my $api = $spec{Xapi};
	if ( ! -f $api ) {
		$EnableAutoConf = "";
	}

	#==========================================
	# Test for root privileges
	#------------------------------------------
	if ($UID != 0) {
		print "xc: only root can do this\n";
		exit(0);
	}
}

#==========================================
# prepare
#------------------------------------------
sub prepare {
	# ...
	# read saved data dictionary hash from file
	# import registry information
	# ---
	my $hashref;
	if (! -s $spec{DbmFile}) {
		print "xc: could not open tree: $spec{DbmFile}\n";
		return -1;
	}
	eval {
		$hashref = retrieve($spec{DbmFile});
	};
	if (! defined $hashref) {
		print "xc: could not open tree: $spec{DbmFile}\n";
		return -1;
	}
	%var = %{$hashref};
	return 0;
}

#==========================================
# save
#------------------------------------------
sub save {
	# ...
	# save hash data as file $spec{DbmFile}
	# ---
	unlink($spec{DbmFile});
	if(!store(\%var,$spec{DbmFile})) {
		print "init: could not create tree: $spec{DbmFile}\n";
		return -1;
	}
	return 0;
}

#==========================================
# HandleTmpSignal
#------------------------------------------
sub HandleTmpSignal {
	# ...
	# signal function to remove tmp stuff
	# ---
	qx (rm -rf /tmp/$TmpDir);
	qx (rm -rf /tmp/sysp-*);
}

#==========================================
# Logger
#------------------------------------------
sub Logger {
	# ...
	# Append log file data to LOG handler...
	# ---
	my $data   = $_[0];
	my $handle = $_[1];
	my $date   = qx (
		LANG=POSIX /bin/date "+%d-%h %H:%M:%S"
	);
	$date =~ s/\n$//;
	if (-f $data) {
		#==========================================
		# got file for logging
		#------------------------------------------
		print $handle "$date <X> Logging File contents: $data [\n";
		my $xf = new FileHandle;
		if ($xf->open("$data")) {
		while (my $line=<$xf>) {
			$line =~ s/^\n//;
			$line =~ s/\n$//;
			$line =~ s/^\t+//g;
			print $handle "\t$line\n";
		}
		}
		print $handle "]\n";
		$xf->close;
	} else {
		#==========================================
		# got string data for logging
		#------------------------------------------
		print $handle "$date <X> ";
		my $lineWrap = 0;
		$data =~ s/^\n//;
		$data =~ s/\n$//;
		if ($data =~ /\n+/) {
			$lineWrap = 1;
		}
		if ($lineWrap) {
			my @list  = split ("\n",$data);
			my $topic = shift (@list);
			print $handle "[ $topic\n";
			foreach (@list) {
				$_ =~ s/^\t+//g;
				print $handle "\t$_\n";
			}
			print $handle "]\n";
		} else {
			print $handle "$data\n";
		}
	}
}

#==========================================
# LinkConfiguration
#------------------------------------------
sub LinkConfiguration {
	# ...
	# this function is called to copy the temporary config
	# file to /etc/X11/xorg.conf and create the symbolic 
	# X links. The links are only created if one of the
	# /usr/X11/bin/X* server exists, for xorg v7 the server
	# is located in /usr/bin and doesn't need to be linked
	# --- 
	my $final  = "/etc/X11/xorg.conf";
	my $save   = "/etc/X11/xorg.conf.saxsave";
	my $server = "/usr/X11R6/bin/XFree86";

	if (! -f $server) {
		$server = "/usr/X11R6/bin/Xorg";
	}
	if (-f $final) {
		qx (cp $final $save);
	}
	#==========================================
	# Save configuration to /etc/X11
	#------------------------------------------
	qx (cp $spec{AutoConf} $final);

	#==========================================
	# Create startup links
	#------------------------------------------
	if ((-f $server) && (! -l "/var/X11R6/bin/X")) {
		qx (rm -f /usr/X11R6/bin/X);
		qx (rm -f /var/X11R6/bin/X);
		qx (ln -s /var/X11R6/bin/X /usr/X11R6/bin/X);
		qx (ln -s $server /var/X11R6/bin/X);
	}
	unlink ( $spec{HWFlag} );
	return ( $final );
}

#==========================================
# Exit
#------------------------------------------
sub Exit {
	# ...
	# exit function wrapper which close the
	# LOG handler first...
	# ---
	my $code = $_[0];
	CloseLog ($logHandle);
	qx (rm -rf $TmpDir);
	exit ($code);
}

#==========================================
# Getty
#------------------------------------------
sub Getty {
	# ...
	# look for the terminal ID and return it. Please
	# note this function has to return a number
	# ---
	my $tty;
	$tty = qx (tty | cut -f2 -dy);
	return($tty);
}

#==========================================
# CheckPID
#------------------------------------------
sub CheckPID {
	# ...
	# check if the given process id still exist in
	# the process table
	# ---
	my $pid = $_[0];
	if ($pid eq "") {
		return 1;
	}
	sleep(2); qx (ps $pid);
	my $exit = $? >> 8;
	return $exit;
}

#==========================================
# OpenLog
#------------------------------------------
sub OpenLog {
	# ...
	# remove old auto configuration file and open a 
	# new log file descriptor...
	# ---
	my $io;
	unlink ($spec{AutoConf});
 
	$io = new FileHandle;
	if (! $io->open(">>$spec{LogFile}")) {
		die "xc: could not create LogFile: $spec{LogFile}";
	}
	my $LOG = $io->fileno;
	open(STDERR,">&$LOG") ||
		die "could not duplicate LOG on stderr";
	binmode($io, ':unix');
	return ($io);
}

#==========================================
# CloseLog
#------------------------------------------
sub CloseLog {
	# ...
	# close current log file descriptor...
	# ---
	my $io = $_[0];
	if (defined $io) {
		$io->close;
	}
}

#==========================================
# Kill
#------------------------------------------
sub Kill {
	# ...
	# This function will try to kill a process with
	# signal 15 first and if it still exists with signal
	# number 9 to be sure it gets killed
	# ---
	my $pid = $_[0];
	my $err;

	if ($pid eq "") {
		return;
	}
	$err = system("kill -15 $pid");
	$err = $? >> 8;
	if ($err > 0) {
		Logger ("die hard on pid: $pid",$logHandle);
		system ("kill -9 $pid");
	}
}

#==========================================
# getParentName
#------------------------------------------
sub getParentName {
	# ...
	# get the name of the parent process calling
	# this script
	# ---
	my @return;
	my $parent = getppid();
	my $name = qx ( cat /proc/$parent/cmdline );
	$name =~ /\/.*\/+(.*)/;
	my @list = split(/\./,$1);
	my $base = shift (@list);
	my $cmdline = qx (
		cat /proc/$parent/cmdline
	);
	chomp ($base);
	push(@return,$base);

	if ($cmdline =~ /.*-font(.*)-iso10646-1/) {
		push(@return,"-fn $1-iso10646-1");
	}
	return (
		@return
	);
}

#==========================================
# HeaderOK
#------------------------------------------
sub HeaderOK {
	# ...
	# check if the header of the config file has been created
	# from SaX2/ISaX... otherwhise this file shouldn't be
	# read in
	# ---
	my $file = "/etc/X11/xorg.conf";
	my $hunk = "SaX generated X.* config file";
	my $msg1 = "SaX: header check failed !\n";
	my $msg2 = "SaX: will not import $file\n";
	if (! -f $file) {
		print $msg1.$msg2;
		return 0;
	}
	if (! open (FD,$file)) {
		print $msg1.$msg2;
		return 0;
	}
	while (<FD>) {
	if ($_ =~ /$hunk/) {
		close FD;
		return 1;
	}
	}
	close FD;
	print $msg1.$msg2;
	return 0;
}

#==========================================
# usage
#------------------------------------------
sub usage {
	# ...
	# usage message for calling xc.pl
	# ---
	print "Linux SaX Version 8.1 level (xc) (2005-09-26)\n";
	print "(C) Copyright 2005 - SUSE LINUX Products GmbH\n";
	print "\n";

	print "usage: xc [ options ]\n";
	print "options:\n";
	print "[ -a | --auto ]\n";
	print "   enable automatic configuration\n";
	print "\n";
	print "[ -x | --xmode ]\n";
	print "   do not use sax calculated Modelines,\n";
	print "   use the server generated once instead\n";
	print "\n";
	print "[ -s | --sysconfig ]\n";
	print "   this option will only take effect if xc\n";
	print "   has to start its own X-Server. It will than\n";
	print "   read the system installed config file\n";
	print "   instead of the HW detection data\n" ;
	print "[ -y | --yast ]\n";
	print "\n";
	print "   start in yast mode which passed the option\n";
	print "   named --yast to the xapi binary\n";
	print "\n";
	print "[ -f | --fullscreen ]\n";
	print "   start the GUI in fullscreen mode\n";
	print "\n";
	print "[ -O | --dialog ]\n";
	print "   pass the option --dialog to the xapi binary\n";
	print "\n";
	print "[ -n | --nointro ]\n";
	print "   skip the greeting message\n";
	print "\n";
	print "[ -p | --nocheckpacs ]\n";
	print "   skip the check for required packages\n";
	print "\n";
	print "[ -h | --help ]\n";
	print "   show this message\n";
	print "--\n";
	Exit(0);
}
