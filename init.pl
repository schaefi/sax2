#!/usr/bin/perl
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
# init.pl configuration level 1
#
# CVS ID:
# --------
# Status: Up-to-date
#
use lib   '/usr/share/sax/modules';
use strict;

use Getopt::Long;
use Storable;
use CheckServer;
use AutoDetect;
use HashMap;
use FileHandle;
use SPPParse;

#==========================================
# Globals
#------------------------------------------
my %var;                # hold configuration parameter
my %spec;               # hold specifications for init.pl
my $StartBatchMode;     # option variable: start batch mode y/n
my $CreateVESAProfile;  # create user profile according to -vesa option
my $IgnoreProfile;      # ignore all automatically added profiles
my $Debug;              # option variable: debugging
my $Virtual;            # option variable: virtual low resolution
my $ModuleList;         # option variable: module(s) to use   
my $AutoMode;           # option variable: auto modeline
my $UseDbmNew;          # use the DbmNew file instead of DbmFile
my $CardNumber;         # option variable: card to use
my $NoIntelMagic;       # don't check for Intel graphics
my $UseGPM;             # option variable: use gpm as repeater
my $MouseDevice;        # option variable: core pointer device
my $MouseProtocol;      # option variable: core pointer protocol
my $HWScanNeeded = 1;   # indicate if scan(),detect() call is needed
my $logHandle;          # log handler
my $D3Answer = "yes";   # answer to the 3D question
my $Quiet = $D3Answer;  # answer questions with [yes|no] [3D...]
my $haveServer;         # Is there a server for later access
my $dpy      = ":0.0";  # display to use

#==========================================
# Globals
#------------------------------------------
our %mop;               # monitor profile hash
our %idp;               # input device profile hash
our %xdp;               # X11 driver profile hash

#==========================================
# Call init...
#------------------------------------------
init();

#==========================================
# init
#------------------------------------------
sub init {
	# ...
	# This function will start creating the fundamental data
	# dictionary named as registry and used in all the following
	# stages of the configuration.
	# ---
	my $depth;   # supported depths
	my $scr;     # screen list
	my $line;    # read line buffer
	my $param;   # gpm parameter
	my @plist;   # pointer list
	my @list;    # raw pointer list 
	my $m;       # helper variable
	my %tmpvar;  # tmp hash

	#==========================================
	# get options...
	#------------------------------------------
	my $result = GetOptions(
		"module|m=s"        => \$ModuleList,
		"chip|c=s"          => \$CardNumber,
		"nointelmagic"      => \$NoIntelMagic,
		"batch|b:s"         => \$StartBatchMode,
		"vesa=s"            => \$CreateVESAProfile,
		"ignoreprofile|i"   => \$IgnoreProfile,
		"debug|d"           => \$Debug,
		"virtual|v"         => \$Virtual,
		"automode|u"        => \$AutoMode,
		"dbmnew"            => \$UseDbmNew,
		"type|t=s"          => \$MouseProtocol,
		"node|n=s"          => \$MouseDevice,
		"quiet|q=s"         => \$Quiet,
		"gpm|g"             => \$UseGPM,
		"help|h"            => \&usage,
		"<>"                => \&usage
	);
	if ( $result != 1 ) {
		usage();
	}
	if ($< != 0) {
		die "init: only root can do this";
	}
	#==========================================
	# check for the Identity.map
	#------------------------------------------
	my $map = "/usr/share/sax/sysp/maps/Identity.map";
	if ( ! -f $map ) {
		die "SaX: no Identity.map found";
	}
	#==========================================
	# check for the GUI and run shell if needed
	#------------------------------------------
	my $api = "/usr/sbin/xapi";
	if ( (! -f $api) && (! defined $ENV{IGNORE_GUI_CHECK}) ) {
		print "SaX: GUI is missing, starting commandline interface\n";
		$StartBatchMode = "";
	}
	#==========================================
	# check the syntax of the ModuleList
	#------------------------------------------
	if ((defined $ModuleList) && ($ModuleList ne "none")) {
		if ($ModuleList !~ /^(\d=\w+)(,\d=\w+)*$/) {
			print "SaX: wrong module syntax...\n";
			print "SaX: syntax: -m CardNr=CardModule[,...]\n";
			exit 1;
		}
	}
	#==========================================
	# build up the spec hash for internal data
	#------------------------------------------
	$spec{StaticFontPathList}  = "/usr/share/sax/api/data/StaticFontPathList";
	$spec{LangFirstPath}       = "/usr/share/sax/api/data/LangFirstPath";
	$spec{LangCodes}           = "/usr/share/sax/api/data/LangCodes";
	$spec{InitFlag}            = "/var/cache/sax/files/reinit";
	$spec{DbmFile}             = "/var/cache/sax/files/config";
	$spec{DbmNew}              = "/var/cache/sax/files/config.new";
	$spec{DbmDir}              = "/var/cache/sax/files";
	$spec{SyspDir}             = "/var/cache/sax/sysp/rdbms";
	$spec{Sysp}                = "/usr/sbin/sysp";
	$spec{LogFile}             = "/var/log/SaX.log";
	$spec{Xmode}               = "/usr/sbin/xmode";
	$spec{RcSysConfigLang}     = "/etc/sysconfig/language";
	$spec{SysConfig}           = "/etc/sysconfig";
	$spec{Profile}             = "/usr/share/sax/pci.pl";
	$spec{ProfileDir}          = "/usr/share/sax/profile/";
	$spec{Monitors}            = "/usr/share/sax/api/data/cdb/Monitors";
	$spec{Corner}              = "/usr/sbin/corner";
	$spec{MD5}                 = "/usr/bin/md5sum";
	$spec{ISaX}                = "/usr/sbin/isax";

	#==========================================
	# check if we have a card, if not exit 
	#------------------------------------------
	my $syspCard = qx ($spec{Sysp} -c);
	if ($syspCard !~ /^Chip:.*/) {
		die "SaX: no graphics card(s) found";
	}
	#==========================================
	# build up the registries fixed data
	#------------------------------------------
	# the auto detection process will add a lots of settings
	# to this default settings as well as special data from
	# profiles may do
	# --
	#==========================================
	# Meta Data specification                  
	#------------------------------------------
	$var{SaXMeta}{0}{SAX_NO_CDB_CHECK} = 1;

	#==========================================
	# Files specification                      
	#------------------------------------------
	$var{Files}{0}{LogFile}    = "";

	#==========================================
	# Module specification                     
	#------------------------------------------
	$var{Module}{0}{Load}      = "dri,dbe,extmod,glx";

	#==========================================
	# ServerFlags specification                
	#------------------------------------------
	$var{ServerFlags}{0}{AllowMouseOpenFail}    = "on";
	$var{ServerFlags}{0}{ZapWarning}            = "on";
	$var{ServerFlags}{0}{"blank time"}          = "";
	$var{ServerFlags}{0}{"standby time"}        = "";
	$var{ServerFlags}{0}{"suspend time"}        = "";
	$var{ServerFlags}{0}{"off time"}            = "";

	#==========================================
	# Keyboard specification                   
	#------------------------------------------
	$var{InputDevice}{0}{Identifier}            = "Keyboard[0]";
	$var{InputDevice}{0}{Driver}                = "kbd";
	$var{InputDevice}{0}{Option}{Protocol}      = "Standard";
	$var{InputDevice}{0}{Option}{XkbRules}      = "xfree86";
	$var{InputDevice}{0}{Option}{XkbModel}      = "pc101";
	$var{InputDevice}{0}{Option}{XkbLayout}     = "us";
	$var{InputDevice}{0}{Option}{XkbVariant}    = "";
	$var{InputDevice}{0}{Option}{AutoRepeat}    = "";
	$var{InputDevice}{0}{Option}{Xleds}         = "";
	$var{InputDevice}{0}{Option}{XkbOptions}    = "";

	#==========================================
	# Mouse specification                      
	#------------------------------------------
	$var{InputDevice}{1}{Identifier}               = "Mouse[1]";
	$var{InputDevice}{1}{Driver}                   = "mouse";
	$var{InputDevice}{1}{Option}{Protocol}         = "Microsoft";
	$var{InputDevice}{1}{Option}{Device}           = "/dev/ttyS0";
	$var{InputDevice}{1}{Option}{SampleRate}       = "";
	$var{InputDevice}{1}{Option}{BaudRate}         = "";
	$var{InputDevice}{1}{Option}{Emulate3Buttons}  = "";
	$var{InputDevice}{1}{Option}{Emulate3Timeout}  = "";
	$var{InputDevice}{1}{Option}{ChordMiddle}      = "";
	$var{InputDevice}{1}{Option}{Buttons}          = "";
	$var{InputDevice}{1}{Option}{Resolution}       = "";
	$var{InputDevice}{1}{Option}{ClearDTR}         = "";
	$var{InputDevice}{1}{Option}{ClearRTS}         = "";
	$var{InputDevice}{1}{Option}{ZAxisMapping}     = "";
	$var{InputDevice}{1}{Option}{MinX}             = "";
	$var{InputDevice}{1}{Option}{MaxX}             = "";
	$var{InputDevice}{1}{Option}{MinY}             = "";
	$var{InputDevice}{1}{Option}{MaxY}             = "";
	$var{InputDevice}{1}{Option}{ScreenNumber}     = "";
	$var{InputDevice}{1}{Option}{ReportingMode}    = "";
	$var{InputDevice}{1}{Option}{ButtonThreshold}  = "";
	$var{InputDevice}{1}{Option}{Buttons}          = "";
	$var{InputDevice}{1}{Option}{SendCoreEvents}   = "";

	#==========================================
	# Monitor specification                    
	#------------------------------------------
	$var{Monitor}{0}{Identifier}                = "Monitor[0]";
	$var{Monitor}{0}{VendorName}                = "Unknown";
	$var{Monitor}{0}{ModelName}                 = "Unknown";
	$var{Monitor}{0}{HorizSync}                 = "29-70";
	$var{Monitor}{0}{VertRefresh}               = "50-90";
	$var{Monitor}{0}{Option}                    = "DPMS";
	$var{Desktop}{0}{CalcModelines}             = "on";

	#==========================================
	# Device specification                     
	#------------------------------------------
	$var{Device}{0}{Identifier}                 = "Device[0]";
	$var{Device}{0}{VendorName}                 = "Unknown";
	$var{Device}{0}{BoardName}                  = "Unknown";
	$var{Device}{0}{Videoram}                   = "";
	$var{Device}{0}{Driver}                     = "vga";
	$var{Device}{0}{Chipset}                    = "";
	$var{Device}{0}{Clocks}                     = "";
	$var{Device}{0}{BusID}                      = "";

	#==========================================
	# Extensions specification                     
	#------------------------------------------
	# $var{Extensions}{0}{Composite}              = "on";

	#==========================================
	# Screen specification                     
	#------------------------------------------
	$var{Screen}{0}{Identifier}                 = "Screen[0]";
	$var{Screen}{0}{Device}                     = $var{Device}{0}{Identifier};
	$var{Screen}{0}{Monitor}                    = $var{Monitor}{0}{Identifier};
	foreach $depth (8,16,24) {
		$var{Screen}{0}{Depth}{$depth}{Modes}      = "800x600";
		$var{Screen}{0}{Depth}{$depth}{ViewPort}   = "";
		$var{Screen}{0}{Depth}{$depth}{Virtual}    = "";
		$var{Screen}{0}{Depth}{$depth}{Visual}     = "";
		$var{Screen}{0}{Depth}{$depth}{Weight}     = ""; # RGB
		$var{Screen}{0}{Depth}{$depth}{Black}      = ""; # RGB 
		$var{Screen}{0}{Depth}{$depth}{White}      = ""; # RGB 
		$var{Screen}{0}{Depth}{$depth}{Option}     = ""; 
	}

	#==========================================
	# ServerLayout specification for Screen0   
	#------------------------------------------
	$var{ServerLayout}{all}{Identifier}            = "Layout[all]";
	$var{ServerLayout}{all}{InputDevice}{0}{id}    = "Keyboard[0]";
	$var{ServerLayout}{all}{InputDevice}{0}{usage} = "CoreKeyboard";
	$var{ServerLayout}{all}{InputDevice}{1}{id}    = "Mouse[1]";
	$var{ServerLayout}{all}{InputDevice}{1}{usage} = "CorePointer";
	$var{ServerLayout}{all}{Screen}{0}{id}         = "Screen[0]";
	$var{ServerLayout}{all}{Screen}{0}{top}        = "";
	$var{ServerLayout}{all}{Screen}{0}{bottom}     = "";
	$var{ServerLayout}{all}{Screen}{0}{left}       = "";
	$var{ServerLayout}{all}{Screen}{0}{right}      = "";

	#==========================================
	# check if we have access to an X-Server 
	#------------------------------------------
	$haveServer = 0;
	my @xpid = GetPids();
	if (@xpid > 0) {
		my @displayStatus = GetDisplay (@xpid,$spec{Corner});
		$dpy = $displayStatus[0];
		if ($displayStatus[1] eq "grant") {
			$haveServer = 1;
			print ("SaX: access to your display has been granted\n");
		} else {
			print ("SaX: access to your display is denied\n");
		}
	} else {
		print ("SaX: no X-Server is running\n");
		print ("SaX: will start own server if needed\n");
	}

	#==========================================
	# search and detect the installed hardware
	# -------------------------------------------
	$logHandle = OpenLog();
	Logger ("Initializing...",$logHandle);
	qx (rm -f $spec{InitFlag});

	my $needUpdate = $ENV{HW_UPDATE};
	if ($needUpdate) {
		qx (rm -f $spec{SyspDir}/*);
		qx (touch $spec{InitFlag});
		qx (touch $spec{DbmDir}/hardware.chg);
		scan();
	}
	#==========================================
	# 3D Detection...
	#------------------------------------------
	# needs to be called first to know about the driver and
	# if this driver supports 3D. The result may influence the
	# rest of the detection so it has to be called first
	# --
	my $result = callSysp ( "3d" );
	if ($needUpdate) {
		Logger ("Sysp: 3D detection data\n$result",
			$logHandle
		);
	} else {
		Logger ("Detection already done
			To perform a fresh detection use the --reinit option",
			$logHandle
		);
	}
	Debug ( $result );

	#==========================================
	# close log file handler
	#------------------------------------------
	CloseLog ( $logHandle );

	#==========================================
	# call detect functions to setup registry
	#------------------------------------------
	detect();
	
	#==========================================
	# Handle option -u | --automode
	#------------------------------------------
	if (defined $AutoMode) {
		foreach $depth (4,8,15,16,24) {
		foreach $scr (keys %{$var{Screen}}) {
		if ($scr !~ /HASH/i) {
			$var{Screen}{$scr}{Depth}{$depth}{Modes} = "Auto";
		}
		}
		}
	} 
	#==========================================
	# handle options --node --type
	#------------------------------------------
	if (defined $MouseDevice) {
		$var{InputDevice}{1}{Option}{Device}   = $MouseDevice;
	}
	if (defined $MouseProtocol) {
		$var{InputDevice}{1}{Option}{Protocol} = $MouseProtocol;
	}
	#==========================================
	# handle option -g | --gpm
	#------------------------------------------
	if (defined $UseGPM) {
		#==========================================
		# start gpm as repeater
		#------------------------------------------
		my %translate;
		$translate{'mouseman'}     = "mman";
		$translate{'microsoft'}    = "ms";
		$translate{'mousesystems'} = "msc";
		$translate{'logitech'}     = "logi";
		$translate{'busmouse'}     = "bm";
		$translate{'ps/2'}         = "ps2";
		$translate{'intellimouse'} = "ms3";
		$translate{'imps/2'}       = "imps2";

		@plist = ();
		my $gpmType   = lc ($var{InputDevice}{1}{Option}{Protocol});
		my $gpmDevice = $var{InputDevice}{1}{Option}{Device};
		if (! defined $translate{$gpmType}) {
			die "SaX: could not translate X11 protocol to GPM protocol...";
		}
		qx (gpm -k 2>/dev/null);
		qx (gpm -t $translate{$gpmType} -m $gpmDevice -R);
		if ( ! defined (qx(pidof gpm 2>/dev/null)) ) {
			die "SaX: could start gpm as repeater...";
		}
		#==========================================
		# use GPM device as input
		#------------------------------------------
		foreach $m (keys %{$var{InputDevice}}) {
		if ($m%2 != 0) {
			$var{InputDevice}{$m}{Option}{Protocol} = "MouseSystems";
			$var{InputDevice}{$m}{Option}{Device}   = "/dev/gpmdata";
		}
		}
	}
	#==========================================
	# save preliminary hash [registry]
	#------------------------------------------
	if (save() == -1) {
		die "init: level 1... abort";
	}
	#==========================================
	# clean profile directory
	#------------------------------------------
	qx (rm -f $spec{ProfileDir}/*.tmp);
	#==========================================
	# handle automatic applied profiles
	#------------------------------------------
	if (! defined $UseDbmNew) {
	if (! defined $IgnoreProfile) {
		my @autoList = ("driver","card","monitor","mouse","keyboard");
		my $include  = 0;
		foreach my $item (@autoList) {
			my %autoProfile = getAutoProfile ( $item );
			foreach my $id (keys %autoProfile) {
				my @profileList = split (/,/,$autoProfile{$id});
				foreach my $profile (@profileList) {
					prepareProfile ( $spec{ProfileDir}.$profile,$id );
					$include = 1;
				}
			}
		}
		if ($include == 1) {
			%var = includeProfile (\%var);
			clearProfile();
		}
	}
	}
	#==========================================
	# Handle option -v | --virtual
	#------------------------------------------
	if (defined $Virtual) {
		$CreateVESAProfile = '0:800x600@60';
	}
	#==========================================
	# Handle option --vesa
	#------------------------------------------
	if (defined $CreateVESAProfile) {
		if ($CreateVESAProfile =~ /(.*):(.*)x(.*)\@(.*)/) {
			my $id  = $1;  # Card ID
			my $rx  = $2;  # X resolution
			my $ry  = $3;  # Y resolution
			my $vs  = $4;  # Vertical Sync value
			my $hs  = int (($ry * $vs)/1000); $hs += 10;
			my $res = $rx."x".$ry;
			addProfileData ("Monitor->$id->VertRefresh=50-$vs");
			addProfileData ("Monitor->$id->HorizSync=20-$hs");
			addProfileData ("Screen->$id->Depth->8->Modes=$res");
			addProfileData ("Screen->$id->Depth->16->Modes=$res");
			addProfileData ("Screen->$id->Depth->15->Modes=$res");
			addProfileData ("Screen->$id->Depth->24->Modes=$res");
			%var = includeProfile (\%var);
			clearProfile();
		}
	}
	#==========================================
	# save preliminary hash [registry]
	#------------------------------------------
	if (save() == -1) {
		die "init: level 1... abort";
	}
	#==========================================
	# handle option -b | --batch
	#------------------------------------------
	if (defined $StartBatchMode) {
		if ($StartBatchMode eq "") {
			print "\n";
			print "Linux SaX Version 8.1 startup level (init)\n";
			print "(C) Copyright 2005 - SUSE LINUX Products GmbH\n";
			print "\n";
		} else {
			my @nameList = split (/,/,$StartBatchMode);
			foreach my $name (@nameList) {
				if ($name !~ /\//) {
					$name = $spec{ProfileDir}.$name;
				}
				prepareProfile ( $name );
			}
		}
	}
	#==========================================
	# Include -b profiles now or provide shell
	#------------------------------------------
	if (defined $StartBatchMode) {
		%var = includeProfile (\%var);
	}
	#==========================================
	# handle option -m | --module
	#------------------------------------------
	# handle the module list (Option -m X=..) with the highest 
	# priority. Therefore we had to check the ModuleList again
	# because it may be modified via a profile
	# ---
	if ($ModuleList ne "none") {
		my %chiplist;
		#==========================================
		# chip{OriginalNumber} = ConfigHashNumber
		# ----------------------------------------
		if ($CardNumber ne "all") {
			my @list  = split (/,/,$CardNumber);
			my $count = 0;
			foreach (@list) {
				$chiplist{$_} = $count;
				$count++;
			}
		} else {
			my @list = split (/,/,$ModuleList);
			foreach (@list) {
				my @pair = split (/=/,$_);
				$chiplist{$pair[0]} = $pair[0];
			}
		}
		#==========================================
		# setup modules
		#------------------------------------------
		my @list = split (/,/,$ModuleList);
		foreach (@list) {
			my @pair = split (/=/,$_);
			$var{Device}{$chiplist{$pair[0]}}{Driver} = $pair[1];
		}
	}
	#==========================================
	# We are ready now. saving hash [registry]
	#------------------------------------------
	if (save() == -1) {
		die "init: level 1... abort";
	}
}

#==========================================
# checkForIntelBoard
#------------------------------------------
sub checkForIntelBoard {
	# ...
	# This function is more or less a hack for Intel onboard
	# graphics. In case of exactly two card devices it checks
	# for a device with the intel driver (i810) assigned. If
	# such a device is found the other card of the two is used
	# exclusively and the Intel device will be disabled. This
	# is done because we assume a user who plugged in another
	# graphics card in a machine with Intel onboard graphics
	# wants to use this card and doesn't want to make use of
	# the onboard graphics at all. It might be possible that
	# this assumption is wrong. Therefore the check can be
	# disabled by the option "--nointelmagic"
	# ---
	my @data = qx (/usr/sbin/sysp -c);
	my $size = @data;
	my $chip = -1;
	if ($size == 2) {
		my $hasIntel = 0;
		foreach my $line (@data) {
			if ($line =~ /(i810|intel)$/) {
				$hasIntel = 1; last;
			}
		}
		if (! $hasIntel) {
			return;
		}
		foreach my $line (@data) {
			$chip++;
			if ($line =~ /(i810|intel)$/) {
				$chip = $chip ^ 1; last;
			}
		}
		if ($chip >= 0) {
			$CardNumber = $chip;
		}
	}
}

#==========================================
# scan
#------------------------------------------
sub scan {
	# ...
	# this function is used to call the system profiler
	# it detects all the needed stuff for an automatic
	# X11 setup. The result is saved into the DBM database
	# called registry and is used again in the autodetect
	# functions
	# ---
	my $result;    # result of sysp -s call
	my $cards;     # grep result
	my $i;         # loop counter
	my @chiplist;  # list of chips given through option -c
	my $used;      # real used number of cards
	my $querystr;  # module name for sysp
	my $subject;   # topic line for log

	if ((! defined $NoIntelMagic) && (! defined $CardNumber)) {
		checkForIntelBoard();
	}

	if (! defined $CardNumber) { $CardNumber = "all"; }
	if (! defined $ModuleList) { $ModuleList = "none"; }

	#==========================================
	# sysp -s mouse...
	#------------------------------------------
	$querystr = "mouse";
	$subject  = "Sysp: Mouse detection data";
	$result   = callSysp ( $querystr );
	Logger ( $subject."\n".$result,$logHandle );
	Debug  ( $result );

	#==========================================
	# sysp -s keyboard
	#------------------------------------------
	$querystr = "keyboard";
	$subject  = "Sysp: Keyboard detection data";
	$result   = callSysp ( $querystr );
	Logger ( $subject."\n".$result,$logHandle );
	Debug  ( $result );

	#==========================================
	# sysp -s server
	#------------------------------------------
	$querystr = "server";
	$subject  = "Sysp: Server detection data";
	if (defined $Quiet) {
		$result   = qx 
		($spec{Sysp} -s $querystr -C $CardNumber -M $ModuleList -A $Quiet);
	} else {
		$result   = qx 
		($spec{Sysp} -s $querystr -C $CardNumber -M $ModuleList);
	}
	chomp $result;
	if ($result =~ /.*Flag.*:.*3D.*/) {
		$D3Answer = "yes";
	}
	Logger ( $subject."\n".$result,$logHandle );
	my $serverLines = split("\n",$result);
	if ($serverLines < 3) {
		die $result;
	}
	Debug ( $result );

	#==========================================
	# sysp -s xstuff
	#------------------------------------------
	$querystr = "xstuff";
	$subject  = "Sysp: XStuff detection data";
	$result = qx ($spec{Sysp} -s $querystr);
	chomp $result;
	Logger ( $subject."\n".$result,$logHandle );
	my $xstuffLines = split("\n",$result);
	if ($xstuffLines < 3) {
		print "$result\n";
		print "SaX: something went wrong while X was called with -probeonly\n";
		print "SaX: try to call 'sax2 -p' and select a single device ?\n";
		die "abort\n";
	}
	Debug ( $result );

	#==========================================
	# switch back to the start terminal  
	#------------------------------------------
	if ($haveServer == 0) {
		my $tty = Getty();
		qx (/usr/bin/chvt $tty);
	}
}

#==========================================
# detect
#------------------------------------------
sub detect {
	# ...
	# this is the detect function. it is called after the
	# scan. detect read from the DBM files and save the 
	# result into the %var hash which is saved using the
	# storable modul.
	# ---
	my $resolutionSet = 0;
	if (defined $UseDbmNew) {
		#==========================================
		# use DbmNew file
		#------------------------------------------
		# retrieve the DbmNew file if it exist instead of
		# using the sysp saved query information
		# --
		if (-f $spec{DbmNew}) {
			my $hashref = retrieve($spec{DbmNew});
			%var = %{$hashref};
			return;
		} else {
			print "SaX: could not open DBM file: $spec{DbmNew}\n";
			print "SaX: revert to standard protocol\n";
		}
	}
	#==========================================
	# use detect functions to setup the hash
	#------------------------------------------
	my $resolutionSet = 0;
	AutoDetectFontPath     (\%spec,\%var);
	AutoDetectMouse        (\%spec,\%var);
	AutoDetectKeyboard     (\%spec,\%var);
	AutoDetectServer       (\%spec,\%var);
	AutoDetectOptions      (\%spec,\%var);

	#==========================================
	# handle CDB overwrite settings
	#------------------------------------------
	# if the monitor detection was able to detect the Resolution 
	# for the monitor via the CDB we need to call the function
	# again after the resolution detection was done
	# --
	$resolutionSet = AutoDetectSyncRanges (\%spec,\%var);
	AutoDetectResolution (\%spec,\%var);
	if ($resolutionSet) {
		ResetLDNr();
		AutoDetectSyncRanges (\%spec,\%var);
	}
	AutoDetectServerLayout (\%spec,\%var);
}

#==========================================
# save
#------------------------------------------
sub save {
	# ...
	# this function is used to save the hash on 
	# disk. The data in this file is called registry
	# ---
	unlink($spec{DbmFile});
	if(! store(\%var,"$spec{DbmFile}")) {
		print "SaX: could not create tree: $spec{DbmFile}\n";
		return (-1);
	}
	return (0);
}

#==========================================
# Logger
#------------------------------------------
sub Logger {
	# ...
	# append log file data to LOG handler...
	# ---
	my $data   = $_[0];
	my $handle = $_[1];
	my $date   = qx (
		LANG=POSIX /bin/date "+%d-%h %H:%M:%S"
	);
	my $lineWrap = 0;
	$data =~ s/^\n//;
	$data =~ s/\n$//;
	$date =~ s/\n$//;
	if ($data =~ /\n+/) {
		$lineWrap = 1;
	}
	print $handle "$date <I> ";
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

#==========================================
# getAutoProfile
#------------------------------------------
sub getAutoProfile {
	# ...
	# This function will check for profiles defined for
	# the given division. The function will return a hash
	# containing the profile assigned to a card ID
	# ---
	my $division = $_[0];
	my %data = {};
	#==========================================
	# Handle card profiles
	#------------------------------------------
	if ($division eq "card") {
		my $profile = qx ($spec{Profile} -p);
		my @list = split(/\n/,$profile);
		foreach my $card (@list) {
			$card =~ s/Chip://g;
			my @line = split(/:/,$card);
			if ($line[1] ne "unknown") {
				$data{$line[0]} = $line[1];
			}
		}
		return %data;
	}
	#==========================================
	# Handle monitor profiles
	#------------------------------------------
	if ($division eq "monitor") {
		foreach (keys %{$mop{MonitorProfile}}) {
		if ($mop{MonitorProfile}{$_} ne "unknown") {
			$data{$_} = $mop{MonitorProfile}{$_};
		}
		}
		return %data;
	}
	#==========================================
	# Handle mouse profiles
	#------------------------------------------
	if ($division eq "mouse") {
		foreach (keys %{$idp{MouseProfile}}) {
		if ($idp{MouseProfile}{$_} ne "<undefined>") {
			$data{$_} = $idp{MouseProfile}{$_};
		}
		}
		return %data;
	}
	#==========================================
	# Handle keyboard profiles
	#------------------------------------------
	if ($division eq "keyboard") {
		foreach (keys %{$idp{KeyboardProfile}}) {
		if ($idp{KeyboardProfile}{$_} ne "<undefined>") {
			$data{$_} = $idp{KeyboardProfile}{$_};
		}
		}
		return %data;
	}
	#==========================================
	# Handle driver profiles
	#------------------------------------------
	if ($division eq "driver") {
		foreach (keys %{$xdp{DriverProfile}}) {
		if ($xdp{DriverProfile}{$_} ne "<undefined>") {
			$data{$_} = $xdp{DriverProfile}{$_};
		}
		}
		return %data;
	}
	return %data;
}

#==========================================
# Getty
#------------------------------------------
sub Getty {
	# ...
	# look for the terminal ID and return it. This
	# function is used to call chvt() on the current
	# terminal id
	# ---
	my $tty;
	$tty = qx(tty | cut -f2 -dy);
	if ($tty =~ /.*dev.*/) {
		return(7);
	}
	return($tty);
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
	unlink($spec{AutoConf});

	$io = new FileHandle;
	if (! $io->open(">>$spec{LogFile}")) {
		die "SaX: could not create LogFile: $spec{LogFile}";
	}
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
	$io->close;
}

#==========================================
# Debug
#------------------------------------------
sub Debug {
	# ...
	# print given information to STDOUT if
	# $Debug has been set
	# ---
	my $result = $_[0];
	if (defined $Debug) {
		print "$result\n";
	}
}

#==========================================
# callSysp
#------------------------------------------
sub callSysp {
	# ...
	# This function will call sysp with the given module
	# and return the result in one string
	# ---
	my $module = $_[0];
	my $result = qx ( $spec{Sysp} -s $module );
	chomp  $result;
	return $result;
}

#==========================================
# usage
#------------------------------------------
sub usage {
	# ...
	# usage message for calling init.pl
	# ---
	print "Linux SaX Version 8.1 level (init) (2005-09-26)\n";
	print "(C) Copyright 2005 - SUSE LINUX Products GmbH\n";
	print "\n";

	print "usage: init [ options ]\n";
	print "options:\n";
	print "[ -b | --batch [ filename,filename,... ]]\n";
	print "   enable batch mode to set configuration options\n";
	print "   via the SaX2 shell. Optionally give a filename holding\n";
	print "   the profile data\n";
	print "\n";
	print "[ -v | --virtual ]\n";
	print "   This option enable the 800x600 virtual mode\n";  
	print "   You should prefere this mode if you don`t want\n"; 
	print "   to use DDC\n";
	print "\n";
	print "[ -V | --vesa < mode-definition > ]\n";
	print "   This option will set a given resolution and vertical sync\n";
	print "   value (in Hz) as VESA standard resolution for a specific\n";
	print "   card. Format:  Card:XxY\@VSync";
	print "   Example: 0:1024x768\@85\n";
	print "\n";
	print "[ -m | --module < name > ]\n";
	print "   This option set the modules to use for the single cards\n";
	print "   setting more than one module require a comma seperated list\n";
	print "\n";
	print "[ -c | --chip < number > ]\n";
	print "   Set the card/chip number(s) to use for the configuration\n";
	print "   If you have a card with more than one graphics chip onboard\n";
	print "   this option may usefull\n";
	print "\n";
	print "[ -d | --debug ]\n";
	print "   enable debug mode, print result to STDOUT\n";
	print "\n";
	print "[ -g | --gpm ]\n";
	print "   enable gpm mode, use gpm as repeater to report\n";
	print "   mouse events over /dev/gpmdata\n";
	print "\n";
	print "[ -n | --node < device-name > ]\n";
	print "   set the device node to use for the core pointer\n";
	print "\n";
	print "[ -t | --type < protocol-name > ]\n";
	print "   set the protocol to use for the core pointer\n";
	print "\n";
	print "[ -q | --quiet ]\n";
	print "   answer any questions with the string given through this\n";
	print "   option, for example the [3D...] question\n";
	print "\n";
	print "[ --help ]\n";
	print "   show this message\n";
	exit(0);
}
