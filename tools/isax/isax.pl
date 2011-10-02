#!/usr/bin/perl
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
# isax.pl ( config writer/reader/adjuster using API data format )
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;
no strict "refs";
no strict "subs";

use lib '/usr/share/sax/modules';

use XFree;
use FBSet;
use ParseConfig;
use CreateSections;
use ImportAPI;
use HashMap;
use Storable;
use ExportAPI;
use Getopt::Long;
use English;

use Storable qw(freeze thaw);

#=========================================
# Globals...
#-----------------------------------------
my $Update;        # Option to update modelines [XFine cache]
my $ListSection;   # Option list section X
my $OutputFile;    # Option give output file name
my $InputFile;     # Option give input api file name
my $Modify;        # Option Modify yes/no
my $Profile;       # Option profile file
my $ProfileDevice; # Option profile device number
my $PrintYCP;      # Option print as YCP if ListSection is active
my $PlusYCP;       # Option print YCP [stderr] plus normal output [stdout]
my $ImportBinary;  # Option import binary storable data
my %init;          # init data hash
my %save;          # save dialog data
my %origin;        # original data during merging
my @key;           # key list in HashMerge function
my @cmd;           # command list in HashMerge function

our %import;       # ImportConfig information
our %dialog;       # dialog data

#----[ init ]------#
sub init {
#----------------------------------------------------------
# test for root privileges and get some options
# create a secure directory and init the variables
# used in other functions
#
	my $result = GetOptions(
		"list|l=s"      => \$ListSection,
		"config|c=s"    => \$OutputFile,
		"file|f=s"      => \$InputFile,
		"modify|m"      => \$Modify,
		"profile|p=s"   => \$Profile,
		"update|u"      => \$Update,
		"device|d=i"    => \$ProfileDevice, 
		"ycp|y"         => \$PrintYCP,
		"plusycp"       => \$PlusYCP,
		"binary|b:s"    => \$ImportBinary,
		"help|h"        => \&usage,
		"<>"            => \&usage
	);
	if ( $result != 1 ) {
		usage();
	}
	if ( ! defined $ListSection ) {
	if ($UID != 0) {
		die "ISaX: only root can do this";
	}
	}
	if (defined $PrintYCP) {
		$PrintYCP = "ycp";
	}
	if (defined $PlusYCP) {
		$PrintYCP = "plusycp";
	}
	# verify option -f is required...
	# --------------------------------
	if ((! defined $InputFile) && (! defined $ListSection)) {
		die "ISaX: no apifile specified";
	}
	# verify option -c is optional...
	# --------------------------------
	if (! defined $OutputFile) {
		$OutputFile = "/etc/X11/xorg.conf";
	}
	$init{TmpDir}      = CreateSecureDir(); 
	$init{Xmode}       = "/usr/sbin/xmode";
	$init{ConfigFile}  = "/etc/X11/xorg.conf";
	$init{BinaryFile}  = "/var/cache/sax/files/config";
	$init{DbmNew}      = "/var/cache/sax/files/config.new";
	$init{XFineCache}  = "/var/cache/xfine";
	$init{ListFile}    = "$init{TmpDir}/apidata";
	$init{ListSection} = $ListSection;
	$init{OutputFile}  = $OutputFile;
	$init{InputFile}   = $InputFile;
	$init{Modify}      = $Modify;

	ApiInit();

	# import specifications...
	# --------------------------
	$import{ApiFile}   = $init{InputFile};
	$import{Xmode}     = $init{Xmode};
}

#----[ main ]----#
sub main {
#----------------------------------------------------------
# main part: use ImportAPI functions to import api 
# file, use xapi functions to create config file
#
	init();
	# list sections...
	# -----------------
	if (defined $init{ListSection}) {
		ListSection($init{ListSection},$PrintYCP);
	}

	# create or modify the configuration...
	# -------------------------------------
	if (! defined $init{ListSection}) {
	if (defined $init{Modify}) {
		ModifyConfiguration();
	} else {
		CreateConfiguration();
	}
	}
	RemoveSecureDir($init{TmpDir});

	# update modelines if set...
	# ---------------------------
	if (defined $Update) {
	IncludeModelineChanges (
		$init{OutputFile},$init{XFineCache}
	);
	}
	exit 0;
}


#=========================================
# Functions...
#-----------------------------------------
#---[ ImportConfig ]-----#
sub ImportConfig {
#----------------------------------------------------------
# read the configuration file and create the
# %dialog hash for further actions...
#
	my $serialized;

	if (! defined $ImportBinary) {
	# /.../
	# we will read the xorg.conf and merge all the
	# information into the global hash structure
	# -------------------------------------------
	my $cp = XFree::ReadConfigFile($init{ConfigFile});
	my $ee = XFree::GetFontPath($cp);
	if ($ee eq "null") {
		die "ISaX: could not import file: $init{ConfigFile}";
	}
	my %files   = ParseFileSection         ($cp);
	my %sflags  = ParseServerFlagsSection  ($cp);
	my %module  = ParseModuleSection       ($cp);
	my %flags   = ParseServerLayoutSection ($cp);
	my %input   = ParseInputDeviceSection  ($cp);
	my %monitor = ParseMonitorSection      ($cp);
	my %modes   = ParseModesSection        ($cp);
	my %device  = ParseDeviceSection       ($cp);
	my %screen  = ParseScreenSection       ($cp);
	my %layout  = ParseServerLayoutSection ($cp);
	my %extend  = ParseExtensionsSection   ($cp);

	%dialog = MergeParseResult(
		\%files,\%module,\%flags ,\%input,\%monitor,
		\%modes,\%device,\%screen,\%layout,\%sflags,\%extend
	);
	} else {
	# /.../
	# we have requested not to read the existing configuration
	# file because --binary is set. We will include the SaX2
	# written binary version of the detection information
	# ----------------------------------------------------- 
	if ($ImportBinary ne "") {
		$init{BinaryFile} = $ImportBinary;
	}
	if (! -s $init{BinaryFile}) {
		die "ISaX: could not import file: $init{BinaryFile}";
	}
	my $hashref = retrieve($init{BinaryFile});
	if (! defined $hashref) {
		die "ISaX: could not open tree: $init{BinaryFile}";
	}
	%dialog = %{$hashref};
	%dialog = ConstructInputOptions (\%dialog);
	}
	PrepareLayoutDefaults();
	PrepareLayout();

	# /.../
	# include profile if given
	# -------------------------
	if (defined $Profile) {
		$serialized = freeze (\%dialog);
		%save   = %{thaw($serialized)};
		%dialog = ImportProfile (
			$Profile,$ProfileDevice,\%dialog
		);
	}
}

#---[ PrepareLayoutDefaults ]----#
sub PrepareLayoutDefaults {
#----------------------------------------------------
# this function is called to set default values to 
# the ServerLayout section
#
	foreach my $id (keys %{$dialog{InputDevice}}) {
		my $lid   = "all";
		my $input = $dialog{InputDevice}{$id}{Identifier};
		foreach my $layoutID (keys %{$dialog{ServerLayout}}) {
		if (defined $dialog{ServerLayout}{$layoutID}{InputDevice}{$id}) {
			$lid = $layoutID;
			last;
		}
		}
		if ($id % 2 == 0) {
			#============================================
			# this are keyboard devices
			#--------------------------------------------
			$dialog{ServerLayout}{$lid}{InputDevice}{$id}{usage}="CoreKeyboard";
			$dialog{ServerLayout}{$lid}{InputDevice}{$id}{id}   =$input;
		} else {
			#============================================
			# this are mouse devices
			#--------------------------------------------
			my $event  = "CorePointer";
			if ($id != 1) {
				$event = "SendCoreEvents";
			}
			if (defined $dialog{ServerLayout}{$lid}{InputDevice}{$id}) {
				$dialog{ServerLayout}{$lid}{InputDevice}{$id}{usage} = $event;
				$dialog{ServerLayout}{$lid}{InputDevice}{$id}{id}    = $input;
			}
		}
	}
	# / ... /
	# look up Xinerama/and Clone mode...
	# -----------------------------------
	foreach my $lid (keys %{$dialog{ServerLayout}}) {
		if (! defined $dialog{ServerLayout}{$lid}{Option}{Xinerama}) {
			$dialog{ServerLayout}{$lid}{Option}{Xinerama}     = "off";
		}
		if ($dialog{ServerLayout}{$lid}{Option}{Xinerama} eq "") {
			$dialog{ServerLayout}{$lid}{Option}{Xinerama}     = "off";
		}
		if (! defined $dialog{ServerLayout}{$lid}{Option}{Clone}) {
			$dialog{ServerLayout}{$lid}{Option}{Clone}        = "off";
		}
		if ($dialog{ServerLayout}{$lid}{Option}{Clone} eq "") {
			$dialog{ServerLayout}{$lid}{Option}{Clone}        = "off";
		}
	}
}

#---[ PrepareLayout ]---#
sub PrepareLayout {
#----------------------------------------------------------
# prepare layout to know every neighbour within each screen 
# line. This will create redundand information but is easier
# to handle within SaX2 later
#
	my %layout;
	foreach my $lid (keys %{$dialog{ServerLayout}}) {

		my %screen = %{$dialog{ServerLayout}{$lid}{Screen}};
		foreach my $id (keys %screen) {
			my $leftOf  = $screen{$id}{left};
			my $rightOf = $screen{$id}{right};
			my $above   = $screen{$id}{top};
			my $beneath = $screen{$id}{bottom};

			if ($leftOf  =~ /Screen\[(.*)\]/) {
				$layout{$id}{right} = $1;
				$layout{$1}{left} = $id;
			}
			if ($rightOf =~ /Screen\[(.*)\]/) {
				$layout{$id}{left} = $1;
				$layout{$1}{right} = $id;
			}
			if ($above   =~ /Screen\[(.*)\]/) {
				$layout{$id}{bottom} = $1;
				$layout{$1}{top} = $id;
			}
			if ($beneath =~ /Screen\[(.*)\]/) {
				$layout{$id}{top} = $1;
				$layout{$1}{bottom} = $id;
			}
		}
		foreach my $id (keys %screen) {
			my $rel = $screen{$id}{relative};
			$dialog{ServerLayout}{$lid}{Screen}{$id}{left}     = "<none>";
			$dialog{ServerLayout}{$lid}{Screen}{$id}{right}    = "<none>";
			$dialog{ServerLayout}{$lid}{Screen}{$id}{top}      = "<none>";
			$dialog{ServerLayout}{$lid}{Screen}{$id}{bottom}   = "<none>";
			$dialog{ServerLayout}{$lid}{Screen}{$id}{relative} = $rel;
			if (defined $layout{$id}{left}) {
				my $n= $layout{$id}{left};
				$dialog{ServerLayout}{$lid}{Screen}{$id}{left}   = "Screen[$n]";
			}
			if (defined $layout{$id}{right}) {
				my $n= $layout{$id}{right};
				$dialog{ServerLayout}{$lid}{Screen}{$id}{right}  = "Screen[$n]";
			}
			if (defined $layout{$id}{top}) {
				my $n= $layout{$id}{top};
				$dialog{ServerLayout}{$lid}{Screen}{$id}{top}    = "Screen[$n]";
			}
			if (defined $layout{$id}{bottom}) {
				my $n= $layout{$id}{bottom};
				$dialog{ServerLayout}{$lid}{Screen}{$id}{bottom} = "Screen[$n]";
			}
		}
	}
}

#---[ CreateApiData ]-----#
sub CreateApiData {
#----------------------------------------------------------
# create API interface file according to the
# imported %dialog data
#
	my @input;    # InputDevice sections
	my @device;   # Device sections
	my @desktop;  # Monitor,Screen sections
	my @layout;   # ServerLayout sections
	my @extend;   # Extensions section

	open(FD,">$init{ListFile}") ||
		die "could not create file $init{ListFile}: $!";
	foreach my $section (keys %dialog) {
	SWITCH: for ($section) {
	# create Keyboard Path and Mouse sections...
	# --------------------------------------------
	/InputDevice/    && do {
		@input   = KeyboardGetInputDevice(); print FD @input;
		@input   = MouseGetInputDevice();    print FD @input;
	last SWITCH;
	};
	# create Card section...
	# ------------------------
	/Device/         && do {
		@device  = CardGetDevice();          print FD @device;
	last SWITCH;
	};
	# create Monitor and Screen sections...
	# --------------------------------------
	/Screen/         && do {
		@desktop = DesktopGetDesktop();      print FD @desktop;
	last SWITCH;
	};
	# create Layout section...
	# -------------------------
	/ServerLayout/   && do {
		@layout  = LayoutGetServerLayout();  print FD @layout;
	last SWITCH;
	};
	# create Extensions section...
    # ----------------------------
	/Extensions/     && do {
		@extend  = ExtensionsGetExtensions();print FD @extend;
	};
	}
	}
	close(FD);
}

#---[ ListSection ]-----#
sub ListSection {
#----------------------------------------------------------
# list the given section as YCP list or in human
# readable format based on the %import hash with
# a tree depth of 1
#
	my $item     = $_[0];
	my $ycp      = $_[1];
 
	my @keylist; # list of id numbers and key values
	my %result;  # result hash in numeric sort order
	my $count;   # modeline counter
	my $lastmode;# number of modelines 
	my $modeline;# current modeline in loop
	my @mlist;   # list of modelines
	my @list;    # YCP list
	my @timing;
	my $k;
	my $v;

	ImportConfig();
	if ((defined $Profile) && ($ListSection ne "Layout")) {
		%dialog = HDif (\%save,\%dialog);
	}
	CreateApiData();

	$import{ApiFile} = $init{ListFile};
	%import = ApiRead();

	# /.../
	# if the desktop section should be listed we will check 
	# if the driver used is the fbdev driver... if yes get the
	# currently used framebuffer resolution and color depth and
	# build the Modes:<X> = <Y> line
	# -------------------------------
	if ($item eq "Desktop") {
		%import = CheckFramebufferModes (\%import);
	}

	foreach (keys %{$import{$item}}) {
		@keylist = split(/ +/,$_);
		$result{$keylist[0]}{$keylist[1]} = $import{$item}{$_};
	}

	# /.../
	# print out YCP or human readable or both
	# ---
	SWITCH: for ($PrintYCP) {
		/^ycp/  && do {
		doYCP (\%result,STDOUT,$item);
		last SWITCH;
		};

		/^plusycp/ && do {
		doHumandReadable (\%result,STDERR);
		doYCP (\%result,STDOUT,$item);
		last SWITCH;
		};

		# default...
		# ----------
		doHumandReadable (\%result,STDOUT);
	}
	unlink($init{ListFile});
}

#---[ doHumandReadable ]------#
sub doHumandReadable {
#----------------------------------------------------------
# print the given hash as human readable list to the 
# given channel
#
	my %result  = %{$_[0]};
	my $channel = $_[1];
	my $id;     # section id
	my $key;    # section key	

	foreach $id (sort keys %result) {
	foreach $key (sort keys %{$result{$id}}) {
		my $ISaXKey = $key;
		if ($ISaXKey eq "DefaultDepth") { 
			$ISaXKey = "ColorDepth"; 
		}
		my $line = sprintf ("%-2s: %-20s: %-40s\n",
			$id,$ISaXKey,$result{$id}{$key}
		);
		print $channel $line;
	}
	}
}

#---[ doYCP ]------#
sub doYCP {
#----------------------------------------------------------
# print the given hash as YCP list to the given channel
#
	my %result   = %{$_[0]};
	my $channel  = $_[1];
	my $item     = $_[2];

	my @keylist; # list of id numbers and key values
	my $id;      # section id
	my $key;     # section key
	my $count;   # modeline counter
	my $lastmode;# number of modelines 
	my $modeline;# current modeline in loop
	my @mlist;   # list of modelines
	my @list;    # YCP list
	my @timing;
	my $k;
	my $v;

	@list = sort keys(%result);
	my $lastid = pop(@list);

	print $channel "\$\[\n";
	foreach $id (sort keys %result) {
		my @list = sort keys(%{$result{$id}});
		my $lastit = pop(@list);
		print $channel "  $id : \$\[\n";
		print $channel "   \"$item\" : \$\[\n";
		foreach $key (sort keys %{$result{$id}}) {
		$k = "\"$key\"";
		$v = "\"$result{$id}{$key}\"";
		if ($key eq "Modelines") {
			# special key modelines needs a new YCP map
			# ------------------------------------------
			print $channel "     $k : \$[\n";
			@mlist = split(/,/,$v);
			$count = 0;
			$lastmode = @mlist;
			foreach $modeline (@mlist) {
				$modeline =~ s/^\"//;
				$modeline =~ s/\"$//;
				$modeline =~ s/^Modeline//;
				$modeline =~ s/\"(.*)\"/\"$1\" : \"Modeline \\\"$1\\\"/;
				@timing   = split(/:/,$modeline);
				my $line  = sprintf("     %-25s : %s",$timing[0],$timing[1]);
				print $channel $line;
				$count++;
				if ($count != $lastmode) {
					print $channel "\",\n";
				} else {
					print $channel "\"\n";
				}
			}
			print $channel "     \],\n";
		} else {
			# normal case...
			# ---------------
			# /.../
			# if the value contains a \" sign we have to 
			# escape this sign otherwhise YaST2 will 
			# not accept the line 
			# ---------------------
			$v = $result{$id}{$key};
			$v =~ s/\"/\\"/g;
			$v = "\"$v\"";
			if ($k eq "\"DefaultDepth\"") {
				$k = "\"ColorDepth\"";
			}
			if ($key ne $lastit) {
				my $line = sprintf("     %-25s : %s,\n",$k,$v);
				print $channel $line;
			} else {
				my $line = sprintf("     %-25s : %s\n",$k,$v);
				print $channel $line;
			}
		}
		}
		print $channel "   \]\n";
		if ($id ne $lastid) {
			print $channel "  \],\n";
		} else {
			print $channel "  \]\n";
		}
	}
	print $channel "\]\n";
}

#---[ CreateConfigFile ]------#
sub CreateConfigFile {
#----------------------------------------------------------
# create the configuration file according to the
# imported API hash...
#
	my %input = %{$_[0]};
	my %store = {};
 
	my @part0;   # header
	my @part1;   # Files
	my @part2;   # ServerFlags
	my @part3;   # Module
	my @part4;   # InputDevice (keyboard)
	my @part5;   # InputDevice (mouse)
	my @part6;   # Monitor
	my @part7;   # Modes
	my @part8;   # Screen
	my @part9;   # Device
	my @part10;  # Monitor
	my @part11;  # ServerLayout
	my @part12;  # DRI
	my @part13;  # Extensions
	my %config;  # section hash

	# Files,ServerFlags,Module...
	# ---------------------------
	%config = ApiImportPath(\%input);
	%store  = HashMerge(\%store,\%config);
	@part0  = CreateHeaderSection("ISaX");
	@part1  = CreateFilesSection(\%config);
	@part2  = CreateServerFlagsSection(\%config);
	@part3  = CreateModuleSection(\%config);

	# InputDevice (Keyboard)...
	# --------------------------
	%config = ApiImportKeyboard(\%input);
	%store  = HashMerge(\%store,\%config);
	@part4  = CreateInputDeviceSection(\%config);

	# InputDevice (Mouse,Tablets,etc)...
	# -----------------------------------
	%config = ApiImportMouse(\%input);
	%store  = HashMerge(\%store,\%config);
	@part5  = CreateInputDeviceSection(\%config);

	# Monitor Modes, Screen...
	# -------------------------
	%config = ApiImportDesktop(\%input);
	%store  = HashMerge(\%store,\%config);
	@part6  = CreateMonitorSection(\%config);
	@part7  = CreateModesSection(\%config);
	@part8  = CreateScreenSection(\%config);

	# Devices...
	# -----------
	%config = ApiImportCard(\%input);
	%store  = HashMerge(\%store,\%config);
	@part9  = CreateDeviceSection(\%config);
	@part10 = CreateMonitorSection(\%config,"yes");

	# ServerLayout...
	# ----------------
	%config = ApiImportLayout(\%input);
	%store  = HashMerge(\%store,\%config);
	@part11 = CreateServerLayoutSection(\%config);
	@part12 = CreateDRISection(\%config);

	%config = ApiImportExtensions(\%input);
	@part13 = CreateExtensionsSection(\%config);

	# /.../
	# save new configuration as storable file
	# this file may be used for later access
	# ----
	store (\%store,$init{DbmNew});

	# create configuration file
	# --------------------------
	open(HANDLE,">$init{OutputFile}") ||
		die "ISaX: could not create $init{OutputFile}: $!\n";
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
	print HANDLE @part12; print HANDLE "\n";
	print HANDLE @part13; print HANDLE "\n"; 
	close(HANDLE);

	if (ImportXFineCache() eq "yes") {
	IncludeModelineChanges (
		$init{OutputFile},$init{XFineCache}
	);
	}
	SaveAsHostConfig (
		$init{OutputFile}
	);
	CreateChecksum (
		$init{OutputFile}
	);
}

#---[ SaveAsHostConfig ]-------#
sub SaveAsHostConfig {
#-------------------------------------------------------
# this function will save the given file as a machine
# name based configuration file. On a diskless client
# using the same NFS share you can use this version of
# the configuration file to start your server
#
	my $basename = $_[0];
	my $gethost  = "/bin/hostname";
	if (-f $basename) {
	if (-f $gethost) {
		my $hostname = qx ($gethost);
		$hostname = $basename."-".$hostname;
		qx (cp $basename $hostname);
	}
	}
}

#---[ CreateChecksum ]-------#
sub CreateChecksum {
#-------------------------------------------------------
# this function will create a MD5 checksum from the
# configuration file created
#
	my $file = $_[0];
	my $md5  = "$file.md5";
	my $sum = qx (/usr/bin/md5sum $file);
	$sum = quotemeta ($sum);
	if ($sum =~ /(.*) +/) {
		$sum = $1;
		open (FD,">$md5") || return;
		print FD $sum;
		close FD;
	}	
}
 
#---[ ModifyConfiguration ]-----#
sub ModifyConfiguration {
#----------------------------------------------------------
# modify a existing configuration using the
# given api modify file information...
#
	my %input;   # merged api file of merge and origin
	my %merge;   # modification information
	my $origin;  # original api file of existing config

	ImportConfig(); 
	CreateApiData();

	$import{ApiFile} = $init{ListFile};
	%origin = ApiRead();
	%origin = CheckFramebufferModes (\%origin);

	$import{ApiFile} = $init{InputFile};
	%merge  = ApiRead();
	%input  = HashMerge(\%origin,\%merge);

	CreateConfigFile(\%input);
	unlink($init{ListFile});
}

#---[ CreateConfiguration ]-----#
sub CreateConfiguration {
#----------------------------------------------------------
# create a configuration from scratch using the
# given api file information...
#
	my %input = ApiRead();
	CreateConfigFile(\%input);
}

#---[ usage ]-------# 
sub usage {
#----------------------------------------------------------
# how to use the ISaX...
#
	print "Linux ISaX Version 2.1 (2001-06-19)\n";
	print "(C) Copyright 2001 SuSE GmbH\n";
	print "\n";

	print "usage: ISaX -f <apifile> [ options ]\n";
	print "options:\n";
	print "[ -f | --file < apifile > ]\n";
	print "  set name of the file containing the\n";
	print "  config specifications/modifications \n";
	print "[ -m | --modify ]\n";
	print "  indicate the apifile to contain only\n";
	print "  information about modifications\n";
	print "[ -u | --update ]\n";
	print "  include modeline patches to the configuration\n";
	print "  file. The modeline cache is located in:\n";
	print "  -- /var/cache/xfine --\n";
	print "[ -l | --list < section > ]\n";
	print "  list the current information about the\n";
	print "  given configuration section. The following\n";
	print "  sections are available:\n";
	print "  /.../\n";
	print "     Card       -> Section Device\n";
	print "     Desktop    -> Section Monitor,Modes,Screen\n";
	print "     Keyboard   -> Section InputDevice\n";
	print "     Mouse      -> Section InputDevice\n";
	print "     Path       -> Section Files,Module,ServerFlags\n";
	print "     Layout     -> Section ServerLayout\n";
	print "     Extensions -> Section Extensions\n";
	print "  /.../\n";
	print "[ -y | --ycp ]\n";
	print "  suppress normal output of a list command\n";
	print "  print a ycp list instead of a human readable\n";
	print "  output\n";
	print "[ -c | --config < output file > ]\n";
	print "  set name of output file default is\n";
	print "  /etc/X11/xorg.conf\n";
	print "[ -p | --profile < file name > ]\n";
	print "  set profile to include for this isax call\n";
	print "  Note:\n";
	print "  -----\n";
	print "  only the differences between the section\n";
	print "  with and without the profile are printed\n";
	print "  The options -p and -d will only take effect\n";
	print "  in combination with the --list option\n";
	print "[ -d | --device < device number >\n";
	print "  for profiles only:\n";
	print "  set the device number the profile should\n";
	print "  take effect on. Default is device nr: 0\n";
	print "[ -h | --help ]\n";
	print "  show this message\n";
	exit(0);
}

#---[ HashMerge ]--------#
sub HashMerge {
#----------------------------------------------------------
# merge data of dest hash into the source
# hash and return the modified source hash
#
	my (%source) = %{$_[0]};
	my (%dest)   = %{$_[1]};

	my $k;       # key
	my $v;       # value 
	my $ref;     # reference
	my $a;       # joined keys
 
	while(($k,$v) = each(%dest)) {
	@key = ();
	push(@key,$k);
	if (ref($v) eq "HASH") {
		$ref = $dest{$k};
		TreeBranch($ref);
	} else {
		$a   = join("}{",@key);
		$a   =~ s/}/'}/g;
		$a   =~ s/{/{'/g;
		my $command = "\$source{'$a'}='$v'";
		eval($command);
	}
	}
	return(%source);
 	# /.../ 
	# Inline sub routine for recursive call
	# --------------------------------------
	sub TreeBranch {
		my $ref  = $_[0];
		my $k;
		my $v;
		my $a;

		while(($k,$v) = each(%{$ref})) {
		push(@key,$k);
		if (ref($v) eq "HASH") {
			TreeBranch($v);
		} else {
			$a   = join("}{",@key);
			$a   =~ s/}/'}/g;
			$a   =~ s/{/{'/g;
			my $command = "\$source{'$a'}='$v'";
			eval($command);
		}
		pop(@key);
		}
		return;
	}
}

#---[ ImportProfile ]-------#
sub ImportProfile {
#----------------------------------------------------------
# import profile information and return the
# changes imported into the given hash
#
	my $profile = $_[0];      # profile file name
	my $device  = $_[1];      # base device number 
	my %source  = %{$_[2]};   # source hash

	my $l;         # line
	my $new;       # new device number
	my @result;    # result as list
	my @add;       # new sections through the profile
	my $newsec;    # loop counter for new sections
	my $data;      # raw profile key
	my $line;      # raw profile data
	my @v;         # list of keys (->)
	my $search;    # formatted search string
	my $size;      # size of @v
	my $ViewValue; # Tree view
	my $ViewRef;   # Tree view reference
	my $key;       # equals $data
	my $value;     # equals $ViewValue
	my $str;       # join "->" @v

	# check device...
	# ----------------
	if (! defined $device) {
		$device = 0;
	}
	# check file...
	# --------------
	if (! -f $profile) {
		return (%source);
	}
	# read profile data...
	# ---------------------
	open(DATA,"$profile");
	while($l = <DATA>) {
	chomp($l);
	SWITCH: for ($l) {
	/^.*\[X\].*/         && do {
		$l =~ s/\[X\]/$device/g;
	};

	/^.*\[X\+([1-9]).*/  && do {
		$new = $device + $1;
		$l =~ s/\[X\+[1-9]\]/$new/g;
		push(@add,$new);
	};
	}
	push(@result,$l);
	}
	close(DATA);

	# include the data to the hash.
	# /.../
	# if a profile includes new sections we had to create 
	# space for this sections. Therefore the branch on the
	# old section is moved to the next section ( old +1 )
	# ------------------------------------------------------
	@add = unique (@add);
	foreach $newsec (@add) {
		%source = HMoveBranch(\%source,$newsec);
	}
	foreach (@result) {
	if ($_ =~ /(.*)=.*{(.*)}.*/) {
		$data      = $1;
		$line      = $2;
		$line      =~ s/ +//g;
		@v         = split(/->/,$line);
		$search    = join("->",@v);
		$size      = @v;
		$ViewValue = "";
		$ViewRef   = "";
		$ViewValue = HGetValue(\%source,$search);
		$ViewValue =~ s/^ +//g;
		$ViewValue =~ s/ +$//g;
		$_ = "$data = $ViewValue\n";
	}
	if ($_ =~ /^(.*)=(.*)/) {
		$key       = $1;
		$value     = $2;
		$key       =~ s/ +//g;
		$value     =~ s/^ +//g;
		@v         = split(/->/,$key);
		$str       = join("->",@v);
		%source    = HSetValue(\%source,$str,$value);
	}
	}
	%source = HUpdateServerLayout(\%source); 
	return (%source);
}

#---[ CheckFramebufferModes ]----#
sub CheckFramebufferModes {
#----------------------------------------------------------
# Check if the fbdev driver is used... if yes get the
# currently used framebuffer resolution and color depth
# and build the Modes:<X> = <Y> line
# 
	my %import  = %{$_[0]};
	foreach (keys %{$import{Card}}) {
	my @keylist = split(/ +/,$_);
	if (($keylist[1] eq "Driver") && ($import{Card}{$_} eq "fbdev")) {
		my $resolution = GetFbResolution();
		my $colordepth = GetFbColor();
		if ($colordepth == 32) {
			$colordepth = 24;
		}
		$import{Desktop}{"$keylist[0] Modes:$colordepth"} = $resolution;
		#$import{Desktop}{"$keylist[0] ColorDepth"} = $colordepth;
	}
	}
	return (%import);
}

#---[ unique ]----#
sub unique {
#----------------------------------------------------------
# create unique list of members
#
	my @list = @_;
	my @result;
	my %map;

	sub num { $a <=> $b; };
	foreach (@list) {
	if ($_ =~ /[0-9]+/) {
		$map{$_} = $_;
	}
	}
	@result = sort num keys(%map);
	return(@result);
}

#---[ CreateSecureDir ]-------#
sub CreateSecureDir {
#----------------------------------------------------------
# create secure directory using mktemp
#
	my $dir;
	$dir = qx(mktemp -qd /tmp/isax.XXXXXX);
	$dir =~ s/\n+//g;
	if (! -d $dir) {
		die "ISaX: could not create tmp dir: $dir";
	}
	return($dir);
}

#---[ RemoveSecureDir ]-------#
sub RemoveSecureDir {
#----------------------------------------------------------
# remove secure directory
#
	my $dir = $_[0];
	qx(rm -rf $dir);
}

#---[ GetFbColor ]-----#
sub GetFbColor {
#----------------------------------------------------------
# this function test for the framebuffer
# color depth
#
	my $data = FBSet::FbGetData();
	my $cols = $data->swig_depth_get();
	if ($cols < 8) {
		return 8;
	}
	return $cols;
}

#---[ GetFbResolution ]-----#
sub GetFbResolution {
#----------------------------------------------------------
# this function test for the framebuffer
# resolution
#
	my $data = FBSet::FbGetData();
	my $xres = $data->swig_x_get();
	my $yres = $data->swig_y_get();
	return $xres."x".$yres;
}

#---[ ConstructInputOptions ]-----#
sub ConstructInputOptions {
#----------------------------------------------------------
# refering to the MergeParseResult function we will create
# the RawOption index according to the InputOptions keys
#
	my %input = %{$_[0]};
	my %InputOptions;
	$InputOptions{Protocol}           = 1;
	$InputOptions{Device}             = 1;
	$InputOptions{SampleRate}         = 1;
	$InputOptions{BaudRate}           = 1;
	$InputOptions{Emulate3Buttons}    = 1;
	$InputOptions{Emulate3Timeout}    = 1;
	$InputOptions{ChordMiddle}        = 1;
	$InputOptions{ClearDTR}           = 1;
	$InputOptions{ClearRTS}           = 1;
	$InputOptions{ZAxisMapping}       = 1;
	$InputOptions{MinX}               = 1;
	$InputOptions{MaxX}               = 1;
	$InputOptions{MinY}               = 1;
	$InputOptions{MaxY}               = 1;
	$InputOptions{MaximumYPosition}   = 1;
	$InputOptions{MinimumYPosition}   = 1;
	$InputOptions{MaximumXPosition}   = 1;
	$InputOptions{MinimumXPosition}   = 1;
	$InputOptions{ScreenNumber}       = 1;
	$InputOptions{ScreenNo}           = 1;
	$InputOptions{ReportingMode}      = 1;
	$InputOptions{Rotation}           = 1;
	$InputOptions{ButtonThreshold}    = 1;
	$InputOptions{ButtonNumber}       = 1;
	$InputOptions{Buttons}            = 1;
	$InputOptions{SendCoreEvents}     = 1;
	$InputOptions{Vendor}             = 1;
	$InputOptions{Name}               = 1;
	$InputOptions{Type}               = 1;
	$InputOptions{Mode}               = 1;
	$InputOptions{Load}               = 1;
	$InputOptions{InputFashion}       = 1;
	$InputOptions{Option}             = 1;
	$InputOptions{TopX}               = 1;
	$InputOptions{TopY}               = 1;
	$InputOptions{BottomX}            = 1;
	$InputOptions{BottomY}            = 1;
	$InputOptions{Suppress}           = 1;
	$InputOptions{Serial}             = 1;
	$InputOptions{EmulateWheel}       = 1;
	$InputOptions{EmulateWheelButton} = 1;

	foreach my $id (keys %{$input{InputDevice}}) {
		my $option = "";
		if ($input{InputDevice}{$id}{Identifier} =~ /Mouse\[(.*)\]/) {
			foreach my $opt (keys %{$input{InputDevice}{$id}{Option}}) {
			if (! defined $InputOptions{$opt}) {
				my $optval = $input{InputDevice}{$id}{Option}{$opt};
				if ($optval ne "") {
					$option = "$option,\"$opt\" \"$optval\"";
					$option =~ s/^,//;
					$input{InputDevice}{$id}{Option}{RawOption} = $option;
					delete $input{InputDevice}{$id}{Option}{$opt};
				}
			}
			}
		}
	}
	return %input;
}

# start with main...
# -------------------
main();
