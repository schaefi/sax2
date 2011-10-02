# /.../
# Copyright (c) 2002 SuSE GmbH Nuernberg, Germany.  All rights reserved.
# Author: Marcus Schaefer <sax@suse.de>, 2002
#
# provide functions to be able to export the results of the
# initial steps created via the interface callbacks. This
# module will seperate the API and the interface
#
# CVS ID:
# --------
# Status: Up-to-date
#
use lib '/usr/share/sax/modules';
use lib '/usr/share/sax/api/dialog';

use XFree;
use Storable;

#----[ DLine ]-----#
sub DLine {
#--------------------------------------------
# return a formatted key value line for the
# API interface file
#
	my $id    = $_[0];
	my $key   = $_[1];
	my $value = $_[2];
	return(sprintf ("%-2s %-20s %-5s %-40s\n"," $id","$key","=","$value"));
}

#----[ MergeParseResult ]--------------#
sub MergeParseResult {
#------------------------------------------------------
# this function is used to merge the given hashes into
# one hash... some special adaptions to the original 
# %spec from init.pl are made here. 
#
# --> Original location: XconfigCallBack
#
	my %files   = %{$_[0]};
	my %module  = %{$_[1]}; 
	my %flags   = %{$_[2]};
	my %input   = %{$_[3]};
	my %monitor = %{$_[4]};
	my %modes   = %{$_[5]};
	my %device  = %{$_[6]};
	my %screen  = %{$_[7]};
	my %layout  = %{$_[8]};
	my %sflags  = %{$_[9]};
	my %extend  = %{$_[10]};

	my %result;
	my $count;
	my $key;
	my $setpath;
	my $opt;
	my $name;
	my $optval;
	my $id;
	my $scr;
	my $next;
	my $line;
	my $modenr;
	my $modet;
	my $modename;
	my $mopt;
	my $display;
	my $nextval;
	my $dopt;
	my $doptval;
	my $xview;
	my $yview;
	my $red;
	my $green;
	my $blue;
	my $hashref;
	my $idc;
	my $rawcount;
	my $newopt;
	my $modelines;
	my @flaglist;
	my @extensionslist;
	my @keylist;
	my $dpix;
	my $dpiy;

	# the following list show all the option keys which
	# are not handled as raw options in the Mouse dialog
	# ----------------------------------------------------
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
	$InputOptions{ButtonMapping}      = 1;
	$InputOptions{InvX}               = 1;
	$InputOptions{InvY}               = 1;
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

	# init result hash with the autodetection data...
	# -------------------------------------------------
	eval {
		$hashref = retrieve($var{DbmFile});
	};
	if (defined $hashref) { 
		%result = %{$hashref};
	}

	#------------------------------------------------# 
	# handle result from ParseFileSection...         #
	# -----------------------------------------------#
	sub nmc { $a <=> $b; }
	foreach $key (keys %files) {
	SWITCH: for ($key) {
	/^ModulePath|FontPath|RgbPath/i     && do {
		$setpath = "";
		foreach $count (sort nmc keys %{$files{$key}}) {
			$setpath = "$setpath,$files{$key}{$count}";
		}
		$setpath =~ s/^,//;
		if ($key eq "FontPath") {
			$result{Files}{0}{FontPath} = $setpath;
		}
		if ($key eq "ModulePath") {
			$result{Files}{0}{ModulePath} = $setpath;
		}
		if ($key eq "RgbPath") {
			$result{Files}{0}{RgbPath} = $setpath;
		}
		last SWITCH;
	};
	/^LogFile/i                         && do {
		$result{Files}{0}{LogFile} = $files{$key};
		last SWITCH;
	};
	}
	}
	
	#------------------------------------------------#
	# handle result from ParseModuleSection...       #
	# -----------------------------------------------#
	foreach $key (keys %module) {
	SWITCH: for ($key) {
	/^Load/i          && do {
		my @modlist = ();
		foreach (keys %{$module{$key}}) {
			push(@modlist,$module{$key}{$_});
		}
		$modstring = join(",",@modlist);
		$modstring =~ s/ +//g; 
		$modstring =~ s/^,//; $modstring =~ s/,$//;
		$result{Module}{0}{Load} = join(",",@modlist);
		last SWITCH;
	};
	/^Disable/i       && do {
		my @modlist = ();
		foreach (keys %{$module{$key}}) {
			push(@modlist,$module{$key}{$_});
		}
		$modstring = join(",",@modlist);
		$modstring =~ s/ +//g;
		$modstring =~ s/^,//; $modstring =~ s/,$//;
		$result{Module}{0}{Disable} = join(",",@modlist);
		last SWITCH;
	};
	/^extmod/i        && do {
		$setpath = ""; 
		foreach $count (keys %{$module{$key}{Option}}) {
			$setpath = "$setpath,$module{$key}{Option}{$count}";
		}
		$setpath =~ s/^,//;
		$result{Module}{0}{extmod}{Option} = $setpath;
		last SWITCH;
	};
	}
	}

	#------------------------------------------------#
	# handle result from ParseServerFlagsSection...  #
	# -----------------------------------------------#
	$setpath = "";
	foreach $key (keys %{$sflags{Option}}) {
	if ($sflags{Option}{$key} eq "none") {
		push(@flaglist,$key);
	} else {
		$result{ServerFlags}{0}{$key} = $sflags{Option}{$key};
	}
	}
	$setpath = join(",",@flaglist);
	$result{ServerFlags}{0}{Option} = $setpath;

	#------------------------------------------------#
	# handle result from ParseExtensionsSection...   #
	# -----------------------------------------------#
	$setpath = "";
	foreach $key (keys %{$extend{Option}}) {
	if ($extend{Option}{$key} eq "none") {
		push(@extensionslist,$key);
	} else {
		$result{Extensions}{0}{$key} = $extend{Option}{$key};
	}	
	}
	$setpath = join(",",@extensionslist);
	$result{Extensions}{0}{Option} = $setpath;

	#------------------------------------------------#
	# handle result from ParseInputDeviceSection...  #
	# -----------------------------------------------#
	# if input is defined delete the auto detection data...
	# --------------------------------------------------------
	@keylist = keys %input;
	if (@keylist > 0) {
		delete $result{InputDevice}; 
	} 
	foreach $key (keys %input) {
	foreach $count (keys %{$input{$key}}) {
	if ($input{$key}{$count}{Identifier} =~ /[Mouse Keyboard]\[(.*)\]/) {
		$idc = $1;
		my $option = "";
		foreach $name (keys %{$input{$key}{$count}}) {
		SWITCH: for ($name) {
		/^Option/        &&  do {
			foreach $opt (keys %{$input{$key}{$count}{$name}}) {
			$optval = $input{$key}{$count}{$name}{$opt};
			$optval =~ s/^ +//;
			$optval =~ s/ +$//;
			SWITCH: for ($opt) {  
			/^MinimumXPosition/   && do {
				$opt = "MinX";
				last SWITCH;
			};
			/^MaximumXPosition/   && do {
				$opt = "MaxX";
				last SWITCH;
			};
			/^EmulateWheel$/      && do {
				$optval = $input{$key}{$count}{$name}{EmulateWheelButton};
				last SWITCH;
			};
			/^EmulateWheelButton/ && do {
				$opt = "EmulateWheel";
				last SWITCH;
			}; 
			/^MinimumYPosition/   && do {
				$opt = "MinY";
				last SWITCH;
			};
			/^MaximumYPosition/   && do {
				$opt = "MaxY";
				last SWITCH;
			};
			/^ScreenNo/           && do {
				$opt = "ScreenNumber";
				last SWITCH;
			};
			}
			# /.../
			# if the option is not defined in the InputOptions hash
			# it is handled as a RawOption... [used for mice only]
			# ---
			if ((! defined $InputOptions{$opt}) && ($idc % 2 == 1)) {
				$option = "$option,\"$opt\" \"$optval\"";
				$option =~ s/^,//;
				$result{InputDevice}{$idc}{$name}{RawOption} = $option;
			} else {
				$result{InputDevice}{$idc}{$name}{$opt} = $optval;
			}
			}
			last SWITCH;
		};

		# default value handling...
		# ---------------------------
		$optval = $input{$key}{$count}{$name};
		$optval =~ s/^ +//;
		$optval =~ s/ +$//;
		$result{InputDevice}{$idc}{$name} = $optval;
		}
		}
	}
	}
	}

	#------------------------------------------------#
	# handle result from ParseServerLayoutSection... #
	# -----------------------------------------------#
	# if layout is defined delete the auto detection data...
	# --------------------------------------------------------
	@keylist = keys %layout;
	if (@keylist > 0) { 
		delete $result{ServerLayout}; 
	}
	foreach $key (keys %layout) {
	foreach $count (keys %{$layout{$key}}) {
	if ($layout{$key}{$count}{Identifier} =~ /Layout\[(.*)\]/) {
		my $lid = $1;
		foreach $name (keys %{$layout{$key}{$count}}) {
		SWITCH: for ($name) {
		/^Identifier/i && do {
			my $identString = $layout{$key}{$count}{$name};
			$result{ServerLayout}{$lid}{Identifier} = $identString;
			last SWITCH;
		};
		/^Option/i    && do {
			foreach $opt (keys %{$layout{$key}{$count}{$name}}) {
				$optval = $layout{$key}{$count}{$name}{$opt};
				$result{ServerLayout}{$lid}{Option}{$opt} = $optval;
			}
			last SWITCH;
		};
		/^Screen/i    && do {
			foreach $opt (keys %{$layout{$key}{$count}{$name}}) {
			if ($opt =~ /.*\[(.*)\]/) {
				$id = $1;
				$id =~ s/ +//g;
				$result{ServerLayout}{$lid}{Screen}{$id}{id} = $opt;
				foreach $scr (keys %{$layout{$key}{$count}{$name}{$opt}}) {
					$optval = $layout{$key}{$count}{$name}{$opt}{$scr};
					if ($optval =~ /none/) {
						$result{ServerLayout}{$lid}{Screen}{$id}{$scr}= "";
					} else {
						$result{ServerLayout}{$lid}{Screen}{$id}{$scr}= $optval;
					}
				}
			}
			}    
			last SWITCH;
		};
		/^Input/i     && do {
			foreach $opt (keys %{$layout{$key}{$count}{$name}}) {
			if ($opt =~ /.*\[(.*)\]/) {
				$id = $1; $id =~ s/ +//g;
				$result{ServerLayout}{$lid}{InputDevice}{$id}{id} = $opt;
				$optval = $layout{$key}{$count}{$name}{$opt};
				$result{ServerLayout}{$lid}{InputDevice}{$id}{usage} = $optval;
			}
			}
			last SWITCH;
		};
		# default
		# --------
		$result{ServerLayout}{$lid}{$name} = $layout{$key}{$count}{$name};
		} 
		}
	}
	}
	}

	#------------------------------------------------#
	# handle result from ParseDeviceSection...       #
	# -----------------------------------------------#
	# if device is defined delete the auto detection data...
	# --------------------------------------------------------
	@keylist = keys %device;
	if (@keylist > 0) {
		delete $result{Device}; 
	}
	$rawcount = 0;
	foreach $key (keys %device) {
	foreach $count (keys %{$device{$key}}) {
	foreach $opt (keys %{$device{$key}{$count}}) {
		$optval = $device{$key}{$count}{$opt};
		if ($optval !~ /HASH/i) {
		SWITCH: for ($opt) {
		/^Identifier/      && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		/^Driver/          && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		/^Videoram/        && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		/^BusID/i          && do {
			$result{Device}{$count}{BusID} = $optval;
			last SWITCH;
		};
		/^Rotate/          && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		/^VendorName/      && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		/^BoardName/       && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		/^Ramdac/          && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		/^Chipset/         && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		/^ClockChip/       && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		/^Screen/          && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		/^Clocks/          && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		/^ChipId/          && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		/^ChipRev/          && do {
			$result{Device}{$count}{$opt} = $optval;
			last SWITCH;
		};
		# default is raw data...
		# ----------------------- 
		if ($optval =~ /[0-9]*/) {
			$result{Device}{$count}{Raw}{$rawcount}{$opt} = $optval;
		} else {
			$result{Device}{$count}{Raw}{$rawcount}{$opt} = "\"$optval\"";
		}
		$rawcount++;
		}
		} else {
		# option strings...
		# -------------------
		if ($opt eq "Option") {
			$setpath = "";
			foreach $next (keys %{$device{$key}{$count}{$opt}}) {
				$optval = $device{$key}{$count}{$opt}{$next};
				$optval =~ s/^ +//g;
				$optval =~ s/ $//g;
				if ($optval eq "<none>") {
					$setpath = "$setpath,$next";
				} else {
				SWITCH: for ($next) {
				/^Rotate/   &&  do {
					$result{Device}{$count}{Rotate} = $optval;
					last SWITCH;
				};
				# default raw option entry
				# ------------------------
				my $val = "\"$next\" \"$optval\"";
				$result{Device}{$count}{Raw}{$rawcount}{Option} = $val;
				$rawcount++; 
				}
				}
			}
			$setpath =~ s/^,//;
			$result{Device}{$count}{Option} = $setpath;
		}
		# the dacspeed string is handled as raw data...
		# ----------------------------------------------
		if ($opt eq "Dacspeed") {
			$setpath = "";
			foreach $next (sort nmc keys %{$device{$key}{$count}{$opt}}) {
				$optval = $device{$key}{$count}{$opt}{$next};
				$setpath = "$setpath $optval"; 
			}
			$setpath =~ s/^ +//;
			$result{Device}{$count}{Raw}{$rawcount}{Dacspeed} = $setpath;
			$rawcount++;
		}
		}
	}
	}  
	}

	#------------------------------------------------#
	# handle result from ParseMonitorSection...      #
	# -----------------------------------------------#
	# if modes is defined delete the auto detection data...
	# --------------------------------------------------------
	@keylist = keys %monitor;
	if (@keylist > 0) {
		delete $result{Monitor};
	}
	foreach $key (keys %monitor) {
	foreach $count (keys %{$monitor{$key}}) { 
	foreach $opt (keys %{$monitor{$key}{$count}}) {
		$optval = $monitor{$key}{$count}{$opt};
		SWITCH: for ($opt) {
		/^DisplaySize/i   && do {
			$dpix = int ($monitor{$key}{$count}{$opt}{width});
			$dpiy = int ($monitor{$key}{$count}{$opt}{height});
			$result{Monitor}{$count}{DisplaySize} = "$dpix $dpiy";
			last SWITCH;
		};
		/^Gamma/i         && do {
			# parsed but not used at the time...
			last SWITCH;
		};
		/^Modeline/i      && do {
			$modelines = "";
			foreach $line (keys %{$monitor{$key}{$count}{$opt}}) {
				$modenr = $line;
				$modet  = $modes{$key}{$count}{$opt}{$line};
				if ($modet =~ /([0-9]+)x([0-9]+)(.*)/i) {
					$modename = "\"$1x$2\"";
					$modet    = $3;
					$modename =~ s/ +//g;
					$modet    =~ s/^ +//;
					$modelines = "$modelines,Modeline $modename $modet"; 
				}
			}
			$modelines =~ s/^,+//;
			$result{Desktop}{$count}{Modelines} = $modelines;
			$result{Desktop}{$count}{CalcModelines} = "no";
			last SWITCH;
		};
		/^Option/i        && do {
			$setpath = "";
			foreach $mopt (keys %{$monitor{$key}{$count}{$opt}}) {
				# /.../
				# handle monitor options with a value. At the 
				# the only option with a value is the CalcAlgorithm option
				# created by SaX2 itself. We will not handle this option
				# in SaX2 but we need it in the ISaX
				# ---
				if ($monitor{$key}{$count}{$opt}{$mopt} ne "<none>") {
					my $val = $monitor{$key}{$count}{$opt}{$mopt};
					$result{Monitor}{$count}{$mopt} = $val;
					next;
				}
				$setpath = "$setpath,$mopt";
			}
			$setpath =~ s/^,//;
			$result{Monitor}{$count}{Option}     = $setpath;
			last SWITCH;
		};
		/^Vendor/i        && do {
			$result{Monitor}{$count}{VendorName} = $optval;
			last SWITCH;
		};
		/^Model/i         && do {
			$result{Monitor}{$count}{ModelName}  = $optval;
			last SWITCH;
		};
		# default
		# --------
		$result{Monitor}{$count}{$opt} = $optval;
		}
	}
	}
	}

	#------------------------------------------------#
	# handle result from ParseModesSection...        #
	# -----------------------------------------------#
	foreach $key (keys %modes) {
	foreach $count (keys %{$modes{$key}}) {
		$modelines = "";
		$result{Monitor}{$count}{Identifier} = $modes{$key}{$count}{Identifier};
		foreach $line (keys %{$modes{$key}{$count}{Modeline}}) {
			$modenr = $line;
			$modet  = $modes{$key}{$count}{Modeline}{$line};
			if ($modet =~ /([0-9]+)x([0-9]+)(.*)/i) {
				$modename = "\"$1x$2\"";
				$modet    = $3;
				$modename =~ s/ +//g;
				$modet    =~ s/^ +//;
				$modelines = "$modelines,Modeline $modename $modet";
			}
		}
		$modelines =~ s/^,+//;
		$result{Desktop}{$count}{Modelines} = $modelines;
		$result{Desktop}{$count}{CalcModelines} = "no";
	}
	}

	#------------------------------------------------#
	# handle result from ParseScreenSection...       #
	# -----------------------------------------------#
	# if screen is defined delete the auto detection data...
	# --------------------------------------------------------
	@keylist = keys %screen;
	if (@keylist > 0) {
		delete $result{Screen};
	}
	foreach $key (keys %screen) {
	foreach $count (keys %{$screen{$key}}) {
	foreach $opt (keys %{$screen{$key}{$count}}) {
		$optval = $screen{$key}{$count}{$opt};
		SWITCH: for ($opt) {
		/^Option/i        && do {
			# no options in screen main section at the time...
			last SWITCH;
		};
		/^Display/i       && do {
			foreach $display (keys %{$screen{$key}{$count}{$opt}}) {
			$id = $screen{$key}{$count}{$opt}{$display}{Depth};
			foreach $next (keys %{$screen{$key}{$count}{$opt}{$display}}) {
			$nextval = $screen{$key}{$count}{$opt}{$display}{$next};
			SWITCH: for ($next) {
			/^Depth/i         && do {
				# do nothing with depth value because we already got it...
				last SWITCH;
			};
			/^Modes/i         && do {
				$nextval =~ s/\@/,/g;
				$result{Screen}{$count}{Depth}{$id}{$next} = $nextval;
			}; 
			/^ViewPort|Virtual/i      && do {
				$viewx = $screen{$key}{$count}{$opt}{$display}{$next}{x};
				$viewy = $screen{$key}{$count}{$opt}{$display}{$next}{y};
				$result{Screen}{$count}{Depth}{$id}{$next} = "$viewx $viewy";
				last SWITCH;
			};
			/^Weight|Black|White/i        && do {
				$red   = $screen{$key}{$count}{$opt}{$display}{$next}{red};
				$green = $screen{$key}{$count}{$opt}{$display}{$next}{green};
				$blue  = $screen{$key}{$count}{$opt}{$display}{$next}{blue};
				$result{Screen}{$count}{Depth}{$id}{$next} ="$red $green $blue";
				last SWITCH;
			};
			/^Option/i        && do {
				$setpath = "";
				foreach $dopt (keys %{$screen{$key}{$count}{$opt}{$display}{$next}}) {
					$doptval = $screen{$key}{$count}{$opt}{$display}{$next}{$dopt};
					if ($doptval ne "") {
						$newopt = "\"$doptval\"";
						$dopt   = "\"$dopt\"";
						$result{Screen}{$count}{Depth}{$id}{Special}{Option}{$dopt}=$newopt;
					} else {
						$setpath = "$setpath,$dopt";
					}
				}
				$setpath =~ s/^,//;
				$result{Screen}{$count}{Depth}{$id}{Option} = $setpath;
				last SWITCH;
			};
			# default...
			# -----------
			$result{Screen}{$count}{Depth}{$id}{$next} = $nextval;
			};
			}
			}
			last SWITCH;
		};
		# default
		# --------
		$result{Screen}{$count}{$opt} = $optval; 
		}
		}
		}
	}
	return(%result);
}

#----[ CardGetDevice ]-----#
sub CardGetDevice {
#-------------------------------------------
# this function is called to create the 
# Card part of the API interface file
#
# --> Original location: CardCallBack
# --> Used sections: Device
#
	my $value;
	my $key;
	my $id;
	my @result;
	my $count;
	my $rawstring;
	my $opt;

	# begin API Device interface...
	# -------------------------------
	push(@result,"Card {\n");
	foreach $id (keys %{$dialog{Device}}) {
	foreach $key (keys %{$dialog{Device}{$id}}) {
		$value = $dialog{Device}{$id}{$key};
		if (($value eq "") || ($value =~ /auto/i)) { next; }
		SWITCH: for ($key) {
		/^CardInfoLabel/  && do {
			# ...
			last SWITCH;
		};
		/^Videoram/       && do {
			push(@result,DLine($id,"Memory",$value));
			last SWITCH;
		};
		/^VendorName/     && do {
			push(@result,DLine($id,"Vendor",$value));
			last SWITCH;
		};
		/^BoardName/      && do {
			push(@result,DLine($id,"Name",$value));
			last SWITCH;
		};
		/^Ramdac/         && do {
			push(@result,DLine($id,"DacChip",$value));
			last SWITCH;
		};
		/^Chipset/        && do {
			push(@result,DLine($id,"GraphicsChip",$value));
			last SWITCH;
		};
		/^Raw/            && do {
			$rawstring = "";
			foreach $count (keys %{$dialog{Device}{$id}{Raw}}) {
			foreach $opt (keys %{$dialog{Device}{$id}{Raw}{$count}}) {
				$value = $dialog{Device}{$id}{Raw}{$count}{$opt};
				$rawstring = "$rawstring,$opt $value";
			}
			}
			$rawstring =~ s/^,+//;
			push(@result,DLine($id,"RawData",$rawstring));
			last SWITCH;
		};
		# default...
		# ------------
		push(@result,DLine($id,$key,$value));
		}
	}
	}
	# end API Device interface...
	# -----------------------------
	push(@result,"}\n\n");
	return(@result);
}

#----[ DesktopGetDesktop ]-----#
sub DesktopGetDesktop {
#-------------------------------------------
# this function is called to create the 
# Desktop part of the API interface file
#
# --> Original location: DesktopCallBack
# --> Used sections: Monitor,Screen
#
	my $key;
	my $value;
	my $id;
	my $next;
	my $opt;
	my $rawstring;
	my $depth;
	my @result;

	# begin API Desktop interface...
	# -------------------------------
	push(@result,"Desktop {\n");
	foreach $id (keys %{$dialog{Monitor}}) {
	foreach $key (keys %{$dialog{Monitor}{$id}}) {
		$value = $dialog{Monitor}{$id}{$key};
		if ($value eq "") { next; }
		SWITCH: for ($key) {
		/^Modeline/     &&  do {
			# ...
			last SWITCH;
		};
		/^Option/       &&  do {
			push(@result,DLine($id,"MonitorOptions",$value));
			last SWITCH;
		};
		# default...
		# -----------
		push(@result,DLine($id,$key,$value));
		}
	}
	}
	foreach $id (keys %{$dialog{Desktop}}) { 
	foreach $key (keys %{$dialog{Desktop}{$id}}) {
		$value = $dialog{Desktop}{$id}{$key};
		if ($value eq "") { next; }
		SWITCH: for ($key) {
		/^SpecialResolution/  &&  do {
			# ...
			last SWITCH;
		};
		/^DesktopInfoLabel/   &&  do {
			# ...
			last SWITCH;
		};
		# default...
		# -----------
		push(@result,DLine($id,$key,$value));
		}
	}
	}
	foreach $id (keys %{$dialog{Screen}}) {
	foreach $key (keys %{$dialog{Screen}{$id}}) {
		$value = $dialog{Screen}{$id}{$key};
		if ($value !~ /HASH/i) {
		SWITCH: for ($key) {
		/^DefaultDepth/    && do {
			# /.../
			# if the driver used is called vmware 
			# we are not allowed to include the ColorDepth key
			# but we have to check for the resolution selected
			# from the user to be available in all color depths 
			# --------------------------------------------------
			if ($dialog{Device}{$id}{Driver} eq "vmware") {
				$defaultd = $dialog{Screen}{$id}{DefaultDepth};
				$selected = $dialog{Screen}{$id}{Depth}{$defaultd}{Modes};
				foreach $d (8,15,16,24) {
					$dialog{Screen}{$id}{Depth}{$d}{Modes} = $selected;
				}
			} else {
				push(@result,DLine($id,"ColorDepth",$value));
				last SWITCH;
			}
		};
		# default...
		# -----------
		push(@result,DLine($id,$key,$value));
		}
		} else {
			foreach $depth (keys %{$dialog{Screen}{$id}{$key}}) {
			foreach $next (keys %{$dialog{Screen}{$id}{$key}{$depth}}) {
				$value = $dialog{Screen}{$id}{$key}{$depth}{$next};
				if (
					($value ne "") && ($value !~ /default/) && 
					($value ne "0 0")
				) {
				SWITCH: for ($next) {
				/^Option/                   && do {
					push(@result,DLine($id,"ScreenOptions:$depth",$value));
					last SWITCH;
				};
				/^Modes/                    && do {
					push(@result,DLine($id,"Modes:$depth",$value));
					last SWITCH;
				};
				/^Visual|Virtual|ViewPort/  && do {
					push(@result,DLine($id,"$next:$depth",$value));
					last SWITCH;
				};       
				/^Special/                  && do {
					foreach $opt (keys %{$dialog{Screen}{$id}{$key}{$depth}{$_}{Option}}) {
						$value = $dialog{Screen}{$id}{Depth}{$depth}{Special}{Option}{$opt};
						$rawstring = "$rawstring,Option $opt $value";
					}
					$rawstring =~ s/^,+//;
					if ($rawstring ne "") {
						push(@result,DLine($id,"ScreenRawLine:$depth",$rawstring));
					}
					last SWITCH;
				};
				# default...
				# ------------
				push(@result,DLine($id,$next,$value));
				}
				} 
			}
			}
		}
	} 
	}
	# end API Desktop interface...
	# -----------------------------
	push(@result,"}\n\n");
	return(@result);
}

#----[ MouseGetInputDevice ]-----#
sub MouseGetInputDevice {
#-------------------------------------------
# this function is called to create the 
# Mouse part of the API interface file
#
# --> Original location: MouseCallBack
# --> Used sections: InputDevice
#
	my $value;
	my $id;
	my $key;
	my @result;
	my $identifier;
	my $driver;

	# begin API Mouse interface...
	# -------------------------------
	push(@result,"Mouse {\n");
	foreach $id (keys %{$dialog{InputDevice}}) {
		if ($id % 2 == 0) { next; }
		$identifier = $dialog{InputDevice}{$id}{Identifier};
		$driver     = $dialog{InputDevice}{$id}{Driver};
		if ($identifier ne "") {
			push(@result,DLine($id,"Identifier",$identifier));
		}
		if ($driver ne "") {
			push(@result,DLine($id,"Driver",$driver));
		}
	}
	foreach $id (keys %{$dialog{InputDevice}}) {
	if ($id % 2 == 0) { next; }
	foreach $key (keys %{$dialog{InputDevice}{$id}{Option}}) {
		$value = $dialog{InputDevice}{$id}{Option}{$key};
		if (
			($value ne "")   && ($value !~ /none/) && ($value ne "off") &&
			($value ne "no") && ($value ne "0")
		) {
			if ($key eq "Type") { $key = "TabletType"; }
			if ($key eq "Mode") { $key = "TabletMode"; }
			# /.../
			# there are input device drivers which use other key names
			# for the same kind of option. We will handle this here
			# -------------------------------------------------------
			if (
				($dialog{InputDevice}{$id}{Driver} eq "elographics") ||
				($dialog{InputDevice}{$id}{Driver} eq "mutouch")      
			) {
				if ($key eq "MinX") { $key = "MinimumXPosition"; }
				if ($key eq "MaxX") { $key = "MaximumXPosition"; }
				if ($key eq "MinY") { $key = "MinimumYPosition"; }
				if ($key eq "MaxY") { $key = "MaximumYPosition"; }
			}
			if (
				($dialog{InputDevice}{$id}{Driver} eq "elographics") ||
				($dialog{InputDevice}{$id}{Driver} eq "wacom"  )     ||
				($dialog{InputDevice}{$id}{Driver} eq "mutouch")      
			) {
				if ($key eq "ScreenNumber") { 
					$key = "ScreenNo";
				}
			}
			push(@result,DLine($id,$key,$value));
		}
	}
	}
	# end API Mouse interface...
	# -----------------------------
	push(@result,"}\n\n");
	return(@result);
}

#----[ KeyboardGetInputDevice ]-----#
sub KeyboardGetInputDevice {
#-------------------------------------------
# this function is called to create the 
# Keyboard and Path part of the API interface file
#
# --> Original location: KeyboardCallBack
# --> Used sections: InputDevice,ServerFlags,Files,Module
#
	my $value;
	my $key;
	my $id;
	my @result;
	my $flag;

	# begin API Keyboard interface...
	# ---------------------------------
	push(@result,"Keyboard {\n");
	foreach $id (keys %{$dialog{InputDevice}}) {
	if ($id % 2 == 0) {
		$identifier = $dialog{InputDevice}{$id}{Identifier};
		$driver     = $dialog{InputDevice}{$id}{Driver};
		if ($identifier eq "") { 
			next; 
		}
		push(@result,DLine($id,"Identifier",$identifier));
		push(@result,DLine($id,"Driver",$driver));
		foreach $key (keys %{$dialog{InputDevice}{$id}{Option}}) {
			$value = $dialog{InputDevice}{$id}{Option}{$key};
			if (($value ne "") && ($value !~ /default|off/)) {
				push(@result,DLine($id,$key,$value));
			}
		}  
	}
	}
	push(@result,"}\n\n");

	# begin API Path interface...
	# -----------------------------
	push(@result,"Path {\n");
	foreach $key (keys %{$dialog{ServerFlags}{0}}) {
		$value = $dialog{ServerFlags}{0}{$key};
		if ($key eq "Option") {
			push(@result,DLine("0","ServerFlags",$value));
		} else {
			if ($value ne "") {
				$flag = "$flag\\n$key,$value";
			}
		}
	}
	$flag =~ s/^,+//;
	if ($flag ne "") {
		$flag =~ s/^\\n//;
		push(@result,DLine("0","SpecialFlags",$flag));
	}
	foreach $key (keys %{$dialog{Files}{0}}) { 
		$value = $dialog{Files}{0}{$key};
		$value =~ s/\n//g;
		$value =~ s/ +//g;
		if ($value ne "") {
			push(@result,DLine("0",$key,$value));
		}
	}
	$value = $dialog{Module}{0}{Load};
	push(@result,DLine("0","ModuleLoad",$value));
	$value = $dialog{Module}{0}{Disable};
	push(@result,DLine("0","ModuleDisable",$value));
	push(@result,"}\n\n");
	return(@result);
}

#----[ LayoutGetServerLayout ]-----#
sub LayoutGetServerLayout {
#-----------------------------------------
# this function is called to create the 
# ServerLayout part of the API interface file
#
# --> Original location: LayoutCallBack
# --> Used sections: ServerLayout
#
	my @result;
	my $count;
	my $inputline;
	my $id;
	my $value;
	my $screen;
	my $left;
	my $right;
	my $bottom;
	my $rel;
	my $top;

	my $section    = "ServerLayout";

	# begin API Layout interface...
	# -------------------------------
	push(@result,"Layout {\n");
	foreach my $lid (keys %{$dialog{$section}}) {
		$count = $lid;
		if ($count eq "all") {
			$count = 0;
		} 
		my $identifier = $dialog{$section}{$lid}{Identifier};
		my $xinerama   = $dialog{$section}{$lid}{Option}{Xinerama};
		my $clone      = $dialog{$section}{$lid}{Option}{Clone};
		push(@result,DLine("$count","Identifier",$identifier));
		push(@result,DLine("$count","Xinerama",$xinerama));
		push(@result,DLine("$count","Clone",$clone));

		$inputline = "";
		foreach $id (keys %{$dialog{$section}{$lid}{InputDevice}}) {
			$value = $dialog{$section}{$lid}{InputDevice}{$id}{id};
			if ($value eq "") { 
				next; 
			}
			if ($id % 2 == 0) {
				push(@result,DLine("$count","Keyboard",$value));
			} else {
				$inputline = "$inputline,$value";
			}
		}
		$inputline =~ s/^,+//;
		push(@result,DLine("$count","InputDevice",$inputline));

		foreach $id (keys %{$dialog{$section}{$lid}{Screen}}) {
			$screen = $dialog{$section}{$lid}{Screen}{$id}{id};
			$top    = $dialog{$section}{$lid}{Screen}{$id}{top};
			$left   = $dialog{$section}{$lid}{Screen}{$id}{left};
			$right  = $dialog{$section}{$lid}{Screen}{$id}{right};
			$bottom = $dialog{$section}{$lid}{Screen}{$id}{bottom};
			$rel    = $dialog{$section}{$lid}{Screen}{$id}{relative};
			if ($rel =~ /^(\d+)-(\d+)-(.*)/) {
				push (@result,DLine("$cound","Relative:$screen","$3,$1,$2"));
			}
			push (@result,
				DLine("$count","Screen:$screen","$left $right $top $bottom")
			);
		}
	}
	# end API Layout interface...
	# -----------------------------
	push(@result,"}\n");
	return(@result);
}

#----[ ExtensionsGetExtensions ]----#
sub ExtensionsGetExtensions {
#-----------------------------------------
# this function is called to create the 
# Extensions part of the API interface file
#
# --> Original location: ExtensionsCallBack
# --> Used sections: Extensions
#
	my @result;
	my $exts;
	my $key;
	my $value;

	push(@result,"Extensions {\n");
	foreach $key (keys %{$dialog{Extensions}{0}}) {
		$value = $dialog{Extensions}{0}{$key};
		if ($key eq "Option") {
		if ($value ne "") {
			push(@result,DLine("0","Extensions",$value));
		}
		} else {
			if ($value ne "") {
				$exts = "$exts\\n$key,$value";
			}
		}
	}
	$exts =~ s/^,+//;
	if ($exts ne "") {
		$exts =~ s/^\\n//;
		push(@result,DLine("0","SpecialExtensions",$exts));
	}
	# end API Extensions interface...
	# -----------------------------
	push(@result,"}\n");
	return(@result);
}

1;
