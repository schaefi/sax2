#/.../
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
#
# ParseConfig.pm configuration level 2
# provide parse functions for the single sections 
#
# CVS ID:
# --------
# Status: Up-to-date
#
use XFree;

#----[ ReadConfig (file) ]----#
sub ReadConfig {
#----------------------------------------------
# this function start the parse process
# using the libxf86config.a X11 lib
# 
	my $ptr;
	my $file = $_[0];
	$ptr = XFree::ReadConfigFile($file);
	return($ptr);
}

#----[ CleanParse ]----#
sub CleanParse {
#----------------------------------------------
# this function looks up duplicate Identifier
# entries and removes them
#
	my %parse   = %{$_[0]};
	my $section = $_[1];
	my $count   = $_[2];
	my %result  = ();
	if ($count == 1) {
		return %parse;
	}
	my $idx = $parse{$section}{$count-1}{Identifier};
	foreach my $n (keys %{$parse{$section}}) {
		my $idn = $parse{$section}{$n}{Identifier};
		if (($n ne $count-1) && ($idn eq $idx)) {
			delete $parse{$section}{$n};
		}
	}
	foreach my $n (keys %{$parse{$section}}) {
		my $index = $parse{$section}{$n}{Identifier};
		if ($index =~ /\[(\d+)\]/) {
			$result{$section}{$1} = $parse{$section}{$n};
			delete $parse{$section}{$n};
		}
	}
	return %result;
}

#----[ ParseFileSection (ptr) ]----#
sub ParseFileSection {
#---------------------------------------------
# this function create a hash array
# containing the configuration information
# from the X11 Files section
#
	my $ptr = $_[0];     # config pointer
	my %parse;           # result hash
	my $result;          # result string from an XFree:: call
	my @list;            # item list 
	my $count;           # counter
	my $i;               # loop counter

	#========================================
	# ModulePath...
	#----------------------------------------
	$result = XFree::GetModulePath($ptr);
	if (defined $result) {
		$count  = 0;
		@list   = split(/,/,$result);
		foreach $i (@list) {
			$parse{ModulePath}{$count} = $i; 
			$count++;
		}
	}
	#========================================
	# FontPath...
	#----------------------------------------
	$result = XFree::GetFontPath($ptr);
	if (defined $result) {
		$count  = 0;
		@list   = split(/,/,$result);
		foreach $i (@list) {
			$parse{FontPath}{$count} = $i;
			$count++;
		}
	}
	#========================================
	# RgbPath...
	#---------------------------------------
	if (XFree::GetParserVersion() <= 4) {
		$result = XFree::GetRgbPath($ptr);
		if (defined $result) {
			$count  = 0;
			@list   = split(/,/,$result);
			foreach $i (@list) {
				$parse{RgbPath}{$count} = $i;
				$count++;
			}
		}
	}
	#========================================
	# LogFile...
	#----------------------------------------
	$result = XFree::GetLogFile($ptr);
	if (defined $result) {
		$parse{LogFile} = $result;
	}
	return(%parse);
}

#----[ ParseModuleSection (ptr) ]----#
sub ParseModuleSection {
#---------------------------------------------
# this function create a hash array
# containing the configuration information
# from the X11 Module section
#
	my $ptr = $_[0];     # config pointer
	my %parse;           # result hash
	my $result;          # result string from an XFree:: call
	my @list;            # item list
	my $count;           # counter
	my $i;               # loop counter

	#========================================
	# Load...
	#----------------------------------------
	$result = XFree::GetModuleSpecs($ptr);
	if (defined $result) {
		$count  = 0;
		@list   = split(/,/,$result);
		foreach $i (@list) {
			$parse{Load}{$count} = $i;
			$count++;
		}
	}

	#========================================
	# Disable...
	#----------------------------------------
	$result = XFree::GetModuleDisableSpecs($ptr);
	if (defined $result) {
		$count  = 0;
		@list   = split(/,/,$result);
		foreach $i (@list) {
			$parse{Disable}{$count} = $i;
			$count++;
		}
	}

	#========================================
	# Subsection extmod....
	#----------------------------------------
	$result = XFree::GetModuleSubSpecs($ptr);
	if (defined $result) {
		$count  = 0;
		@list   = split(/:/,$result);
		$result = $list[1];
		if (defined $result) {
		@list   = split(/,/,$result); 
		foreach $i (@list) {
			$parse{extmod}{Option}{$count} = $i;
			$count++;
		}
		}
	}
	return(%parse);
}

#----[ ParseInputDeviceSection (ptr) ]----#
sub ParseInputDeviceSection {
#---------------------------------------------
# this function create a hash array
# containing the configuration information
# from the X11 InputDevice sections
#
	my $ptr = $_[0];     # config pointer
	my %parse;           # result hash
	my $result;          # result string from an XFree:: call
	my @list;            # item list
	my @sections;        # list of InputDevice sections
	my $count;           # counter
	my @option;          # list of option name and value
	my $id;              # Identifier
	my $opt;             # Options
	my $drv;             # Driver
	my $i;               # loop counter
	my $special;         # special XkbOptions value
	my @opt;             # option list 
	my $optlist;         # options as comma seperated list

	$result = XFree::GetInputSpecs($ptr);
	if (defined $result) {
		$count = 0;

		# split sections...
		# -----------------
		@sections  = split(/\|/,$result);
		foreach $i (@sections) {
			$i =~ /id=(.*) driver=(.*) option=(.*)/;
			$id  = $1;
			$drv = $2;
			$opt = $3;
			$id  =~ s/ +//g;
			$drv =~ s/ +//g;

			$parse{Input}{$count}{Identifier} = $id;
			$parse{Input}{$count}{Driver}     = $drv;
 
			# look up XkbOptions first...
			# ----------------------------
			if ($opt =~ /XkbOptions:\§(.*)\§/) {
				$special = $1; $special =~ s/ +//g;
				$parse{Input}{$count}{Option}{XkbOptions} = $special;
				$opt =~ s/XkbOptions:\§$special\§//;
				$opt =~ s/^ +//g; $opt =~ s/ +$//g;
				$opt =~ s/\,$//;
			}
			# look up XkbLayout next...
			# -------------------------
			if ($opt =~ /XkbLayout:(.*),XkbModel/) {
				$special = $1; $special =~ s/ +//g;
				$parse{Input}{$count}{Option}{XkbLayout} = $special;
				$parse{Input}{$count}{Option}{XkbLayout} =~ s/,,/,\!,/g;
				$opt =~ s/,XkbLayout:$special//;
			}
			# look up XkbVariant next...
			# -------------------------
			if ($opt =~ /XkbVariant:(.*)/) {
				$special = $1; $special =~ s/ +//g;
				$parse{Input}{$count}{Option}{XkbVariant} = $special;
				$parse{Input}{$count}{Option}{XkbVariant} =~ s/,,/,\!,/g;
				$opt =~ s/,XkbVariant:$special//;
			}
 
			@list = split(/,/,$opt);
			@opt  = ();
			foreach $n (@list) {
				@option = split(/:/,$n);
				SWITCH: for ($option[0]) {
				/^Tilt/i       && do {
					push(@opt,$option[0]);
					last SWITCH;
				};
				/^USB/i        && do {
					push(@opt,$option[0]);
					last SWITCH;
				};
				/^AlwaysCore/i && do {
					push(@opt,$option[0]);
					last SWITCH;
				};
				/^KeepShape/i  && do {
					push(@opt,$option[0]);
					last SWITCH;
				};
				# default
				# -------
				$parse{Input}{$count}{Option}{$option[0]} = $option[1];
				}
				$optlist = join(",",@opt);
				$optlist =~ s/^,+//; $optlist =~ s/,+$//;
				if ($optlist ne "") {
					$parse{Input}{$count}{Option}{Option} = $optlist;
				}
			} 
			$count++;
		}
	}
	return(%parse);
}

#----[ ParseServerFlagsSection (ptr) ]----#
sub ParseServerFlagsSection {
#---------------------------------------------
# this function create a hash array
# containing the configuration information
# from the X11 ServerFlags section
#
	my $ptr = $_[0];     # config pointer
	my %parse;           # result hash
	my $result;          # result string from an XFree:: call
	my @list;            # item list
	my $count;           # counter
	my $i;               # loop counter

	#========================================
	# Option...
	#----------------------------------------
	$result = XFree::GetFlags($ptr);
	if (defined $result) {
		$count  = 0;
		@list   = split(/,/,$result);
		foreach $i (@list) {
		if ($i =~ /(.*):(.*)/) {
			$optn = $1;
			$optv = $2;
			$parse{Option}{$optn} = $optv;
			$count++;
		}
		}
	}
	return(%parse);
}

#----[ ParseExtensionsSection (ptr) ]----#
sub ParseExtensionsSection {
#---------------------------------------------
# this function create a hash array
# containing the configuration information
# from the X11 Extensions section
#
	my $ptr = $_[0];     # config pointer
	my %parse;           # result hash
	my $result;          # result string from an XFree:: call
	my @list;            # item list
	my $count;           # counter
	my $i;               # loop counter
	
	#========================================
	# Option...
	#----------------------------------------
	$result = XFree::GetExtensions($ptr);
	if (defined $result) {
		$count  = 0;
		@list   = split(/,/,$result);
		foreach $i (@list) {
		if ($i =~ /(.*):(.*)/) {
			$optn = $1;
			$optv = $2;
			$parse{Option}{$optn} = $optv;
			$count++;
		}
		}
	}
	return(%parse);
}

#----[ ParseServerLayoutSection (ptr) ]----#
sub ParseServerLayoutSection {
#---------------------------------------------
# this function create a hash array
# containing the configuration information
# from the X11 ServerLayout sections
#
	my $ptr = $_[0];     # config pointer
	my %parse;           # result hash
	my $result;          # result string from an XFree:: call
	my @list;            # item list
	my $count;           # counter
	my $id;              # identifier part
	my $scr;             # screen part
	my $ip;              # input device part
	my $opt;             # option part
	my @option;          # list of options
	my @screen;          # list of screens
	my @sections;        # list of sections
	my $i;               # loop counter

	$result = XFree::GetLayoutSpecs($ptr);
	if (defined $result) {
		$count = 0;
		#========================================
		# split sections...
		#----------------------------------------
		@sections  = split(/\|/,$result);
		foreach $i (@sections) {
			$i =~ /.*option=(.*)/;
			$opt = $1;
			$i =~ s/option=(.*)//;
			$i =~ /id=(.*) screen=(.*) inputdev=(.*)/;
			$id  = $1;
			$scr = $2;
			$ip  = $3;
			$id  =~ s/ +//g;

			if ($id =~ /.*-(.*)/) {
				$id = $1;
			}
			#$scr =~ s/Screen\[(.*?)\]/\1/g;
			$parse{Layout}{$count}{Identifier} = $id;

			#========================================
			# get options....
			#----------------------------------------
			@list = split(/,/,$opt);
			foreach $n (@list) {
				@option = split(/:/,$n);
				$parse{Layout}{$count}{Option}{$option[0]} = $option[1];
			}
			#========================================
			# get screens...
			#----------------------------------------
			@list = split(/:/,$scr);
			foreach $n (@list) {
				@screen = split(/,/,$n);
				$parse{Layout}{$count}{Screen}{$screen[0]}{top}    = $screen[2];
				$parse{Layout}{$count}{Screen}{$screen[0]}{bottom} = $screen[3];
				$parse{Layout}{$count}{Screen}{$screen[0]}{left}   = $screen[4];
				$parse{Layout}{$count}{Screen}{$screen[0]}{right}  = $screen[5];
				$parse{Layout}{$count}{Screen}{$screen[0]}{relative}=$screen[6];
			}
			#========================================
			# get input devices...
			#----------------------------------------
			@list = split(/:/,$ip);
			foreach $n (@list) {
				@inputdev = split(/,/,$n);
				$parse{Layout}{$count}{Input}{$inputdev[0]} = $inputdev[1];
			}
			$count++;
		}
	}
	return(%parse);
}

#----[ ParseDeviceSection (ptr) ]----#
sub ParseDeviceSection {
#---------------------------------------------
# this function create a hash array
# containing the configuration information
# from the X11 Device sections
#
	my $ptr = $_[0];     # config pointer
	my %parse;           # result hash
	my $result;          # result string from an XFree:: call
	my @list;            # item list
	my $count;           # counter
	my @line;            # element list
	my $n;               # loop counter
	my $l;               # loop counter 
	my $i;               # loop counter
	my @option;          # option list
	my @sections;        # sections
	my @value;           # value string x=y

	$result = XFree::GetDeviceSpecs($ptr);
	if (defined $result) {
		$count = 0;

		# split sections...
		# -----------------
		@sections  = split(/\|/,$result);
		foreach $i (@sections) {

		#==========================================
		# look up board and vendor names, the names 
		# may contain blanks...
		#------------------------------------------
		if ($i =~ /vendor=\"([^\"]*)\"/) {
			$parse{Device}{$count}{VendorName} = $1;
		}
		if ($i =~ /board=\"([^\"]*)\"/) {
			$parse{Device}{$count}{BoardName}  = $1;
		}
		if ($i =~ s/(SaXDualMonitorVendor:.*?,)//) {
			my $vendor = $1;
			$vendor =~ s/,$//;
			my @option = split(/:/,$vendor);
			$parse{Device}{$count}{Option}{$option[0]} = $option[1];
		}
		if ($i =~ s/(SaXDualMonitorModel:.*?,)//) {
			my $model = $1;
			$model =~ s/,$//;
			my @option = split(/:/,$model);
			$parse{Device}{$count}{Option}{$option[0]} = $option[1];
		}
		if ($i =~ s/(SaXExternal:.*?,)//) {
			my $external = $1;
			$external =~ s/,$//;
			my @option = split(/:/,$external);
			$parse{Device}{$count}{Option}{$option[0]} = $option[1];
		}
		if ($i =~ s/(MonitorLayout:.+?,.+?,)//) {
			my $layout = $1;
			$layout =~ s/,$//;
			my @option = split(/:/,$layout);
			$parse{Device}{$count}{Option}{$option[0]} = $option[1];
		}
		if ($i =~ s/(KernelModuleParm:.+=.+?,)//) {
			my $special = $1;
			$special =~ s/,$//;
			my @option = split(/:/,$special);
			$parse{Device}{$count}{Option}{$option[0]} = $option[1];
		}
		if ($i =~s/(DesktopSetup:(?:Horizontal,Reverse|Vertical,Reverse))//){
			my $special = $1;
			$special =~ s/,$//;
			my @option = split(/:/,$special);
			$parse{Device}{$count}{Option}{$option[0]} = $option[1];
		} elsif ($i =~s/(DesktopSetup:(?:Horizontal|Vertical))//) {
			my $special = $1;
			$special =~ s/,$//;
			my @option = split(/:/,$special);
			$parse{Device}{$count}{Option}{$option[0]} = $option[1];
		}
		if ($i =~ s/(ForceMonitors:.+?,.+?,)//) {
			my $special = $1;
			$special =~ s/,$//;
			my @option = split(/:/,$special);
			$parse{Device}{$count}{Option}{$option[0]} = $option[1];
		}

		@line = split(/ /,$i);
		foreach $n (@line) {
			@value = split(/=/,$n);
			if (defined $value[0]) {
			SWITCH: for ($value[0]) {
			#========================================
			# Identifier...
			#----------------------------------------
			/^id/          && do {
			if ($value[1] =~ /.*-(.*)/) {
				$value[1] = $1;
			}
			$parse{Device}{$count}{Identifier} = $value[1]; 
			last SWITCH; 
			};
			#========================================
			# Chipset...
			#----------------------------------------
			/^chipset/     && do { 
				$parse{Device}{$count}{Chipset} = $value[1];
				last SWITCH; 
			};
 			#========================================
			# Screen...
			#----------------------------------------
			/^screen/      && do {
				$parse{Device}{$count}{Screen} = $value[1];
				last SWITCH;
			};
			#========================================
			# Card...
			#---------------------------------------- 
			/^card/        && do { 
				$parse{Device}{$count}{Card} = $value[1]; 
				last SWITCH; 
			};
			#========================================
			# Driver...
			#----------------------------------------
			/^driver/      && do { 
				$parse{Device}{$count}{Driver} = $value[1];
				last SWITCH; 
			};
			#========================================
			# Ramdac...
			#----------------------------------------
			/^ramdac/      && do { 
				$parse{Device}{$count}{Ramdac} = $value[1]; 
				last SWITCH; 
			};
 			#========================================
			# Dacspeed...
			#---------------------------------------- 
			/^dac8/        && do { 
				$parse{Device}{$count}{Dacspeed}{8} = $value[1];
				last SWITCH; 
			};
			/^dac16/       && do { 
				$parse{Device}{$count}{Dacspeed}{16} = $value[1];
				last SWITCH; 
			};
			/^dac24/       && do { 
				$parse{Device}{$count}{Dacspeed}{24} = $value[1];
				last SWITCH; 
			};
			/^dac32/       && do { 
				$parse{Device}{$count}{Dacspeed}{32} = $value[1];
				last SWITCH; 
			};
			#========================================
			# Videoram...
			#---------------------------------------- 
			/^videoram/    && do { 
				$parse{Device}{$count}{Videoram} = $value[1];
				last SWITCH; 
			};
 			#========================================
			# BiosBase...
			#----------------------------------------
			/^biosbase/    && do { 
				$parse{Device}{$count}{BiosBase} = $value[1];
				last SWITCH; 
			};
			#========================================
			# MemBase...
			#----------------------------------------
			/^membase/     && do { 
				$parse{Device}{$count}{MemBase} = $value[1];
				last SWITCH; 
			};
			#========================================
			# IoBase...
			#----------------------------------------
			/^iobase/      && do { 
				$parse{Device}{$count}{IoBase} = $value[1];
				last SWITCH; 
			};
 			#========================================
			# ClockChip....
			#----------------------------------------
			/^clockchip/   && do { 
				$parse{Device}{$count}{ClockChip} = $value[1];
				last SWITCH; 
			};
			#========================================
			# ChipId...
			#----------------------------------------
			/^chipid/      && do { 
				$parse{Device}{$count}{ChipId} = $value[1];
				last SWITCH; 
			};
			#========================================
			# ChipRev...
			#----------------------------------------
			/^chiprev/     && do { 
				$parse{Device}{$count}{ChipRev} = $value[1];
				last SWITCH; 
			};
			#========================================
			# Clocks...
			#----------------------------------------
			/^clocks/      && do { 
				$parse{Device}{$count}{Clocks} = $value[1];
				last SWITCH; 
			};
			#========================================
			# TextClkFreq...
			#----------------------------------------
			/^textclkfreq/ && do { 
				$parse{Device}{$count}{TextClkFreq} = $value[1];
				last SWITCH; 
			};
			#========================================
			# BusId...
			#----------------------------------------
			/^busid/       && do { 
				$parse{Device}{$count}{BusID} = $value[1];
				$parse{Device}{$count}{BusId} = $value[1];
				last SWITCH; 
			};
			#========================================
			# Irq...
			#----------------------------------------
			/^irq/         && do { 
				$parse{Device}{$count}{Irq} = $value[1];
				last SWITCH; 
			};
  			#========================================
			# Option...
			#----------------------------------------
			/^option/      && do {
				my $Mode2;
				my $MetaModes;
				my $ConnectedMonitor;
				if ($value[1] =~ /MetaModes:(.*)/) {
					my @tlist = split (/,SaX/,$1);
					$MetaModes = $tlist[0];
					$value[1] =~ s/MetaModes:$MetaModes,//;
				}
				if ($value[1] =~ /Mode2:(.*)/) {
					my @tlist = split (/,FSA/,$1);
					$Mode2 = $tlist[0];
					$value[1] =~ s/Mode2:$Mode2,//;
				}
				if ($value[1] =~ /ConnectedMonitor:(.*)/) {
					my @tlist = split (/,SaX/,$1);
					$ConnectedMonitor = $tlist[0];
					$value[1] =~ s/ConnectedMonitor:$ConnectedMonitor,//;
				}
				@list = split(/,/,$value[1]);
				if (defined $MetaModes) {
					push (@list,"MetaModes:$MetaModes");
				}
				if (defined $Mode2) {
					push (@list,"Mode2:$Mode2");
				}
				if (defined $ConnectedMonitor) {
					push (@list,"ConnectedMonitor:$ConnectedMonitor");
				}
				foreach $l (@list) {
					if ($l =~ /^MetaModes/) {
						$l =~ s/MetaModes://;
						$parse{Device}{$count}{Option}{MetaModes} = $l;
					} else {
						$l =~ s/,$//;
						@option = split(/:/,$l);
						if (defined $option[0]) {
							$parse{Device}{$count}{Option}{$option[0]} = $option[1];
						}
					}
				}
				last SWITCH; 
			};
			}
			}
		}
		$count++;
		}
	}
	%parse = CleanParse (\%parse,"Device",$count);
	return(%parse);
}

#----[ ParseModesSection (ptr) ]----#
sub ParseModesSection {
#---------------------------------------------
# this function create a hash array
# containing the configuration information
# from the X11 Modes sections
#
	my $ptr = $_[0];     # config pointer
	my %parse;           # result hash
	my $result;          # result string from an XFree:: call
	my @list;            # item list
	my $count;           # counter
	my $i;               # loop counter
	my @sections;        # sections
	my @value;           # value elements x=y
	my @option;          # option elements
	my $l;               # loop counter
	my $modeline;        # modelines
	my @line;            # elements after (/ /,$i)

	$result = XFree::GetModesSpecs($ptr);
	if (defined $result) {
		$count = 0;

		# split sections...
		# -----------------
		@sections  = split(/\|/,$result);
		foreach $i (@sections) { 
		# /.../
		# get modelines first, because the blanks within the 
		# timing parameters cause trouble while splitting with (/ /,$i)
		# --------------------------------------------------------------
		$modeline = $i;
		$modeline =~ /modeline=(.*)/;
		$modeline = $1;
		if ($modeline ne "") {
			$i =~ s/$modeline//;
		}

		@line = split(/ /,$i);
		foreach $n (@line) {
			@value = split(/=/,$n);
			if (defined $value[0]) {
			SWITCH: for ($value[0]) {
			#========================================
			# Identifier...
			#----------------------------------------
			/^id/          && do {
				if ($value[1] =~ /.*-(.*)/) {
					$value[1] = $1;
				}
				$parse{Modes}{$count}{Identifier} = $value[1];
				last SWITCH;
			};
			#========================================
			# Modeline...
			#----------------------------------------
			/^modeline/    && do {
				@list = split(/,/,$modeline);
				$mnr  = 0;
				foreach $l (@list) {
					$parse{Modes}{$count}{Modeline}{$mnr} = $l;
					$mnr++;
				}
				last SWITCH;
			};
			}
			}
		}
		$count++;
		}
	}
	%parse = CleanParse (\%parse,"Modes",$count);
	return(%parse);
}

#----[ ParseMonitorSection (ptr) ]----#
sub ParseMonitorSection {
#---------------------------------------------
# this function create a hash array
# containing the configuration information
# from the X11 Monitor sections
#
	my $ptr = $_[0];     # config pointer
	my %parse;           # result hash
	my $result;          # result string from an XFree:: call
	my @list;            # item list
	my $count;           # counter
	my $i;               # loop counter
	my @sections;        # sections
	my @value;           # value elements x=y
	my @option;          # option elements
	my $l;               # loop counter
	my $modeline;        # modelines
	my @line;            # elements after (/ /,$i)

	$result = XFree::GetMonitorSpecs($ptr);
	if (defined $result) {
		$count = 0;

		# split sections...
		# -----------------
		@sections  = split(/\|/,$result);
		foreach $i (@sections) {
		# /.../
		# get modelines first, because the blanks within the 
		# timing parameters cause trouble while splitting with (/ /,$i)
		# --------------------------------------------------------------
		$modeline = $i;
		$modeline =~ /modeline=(.*)/;
		$modeline = $1;
		if (defined $modeline) {
			$i =~ s/$modeline//;
		}

		@line = split(/ /,$i);
		foreach $n (@line) {
			@value = split(/=/,$n);
			if (defined $value[0]) {
			SWITCH: for ($value[0]) {
			#========================================
			# Identifier...
			#----------------------------------------
			/^id/          && do {
				if ($value[1] =~ /.*-(.*)/) {
					$value[1] = $1;
				}
				$parse{Monitor}{$count}{Identifier} = $value[1];
				last SWITCH;
			};
			#========================================
			# Vendor...
			#----------------------------------------
			/^vendor/          && do {
				$i =~ /.*vendor=(.*)model=.*/;
				$value[1] = $1;
				$parse{Monitor}{$count}{Vendor} = $value[1];
				last SWITCH;
			};
			#========================================
			# DisplaySize...
			#----------------------------------------
			/^width/          && do {
				$parse{Monitor}{$count}{DisplaySize}{width} = $value[1];
				last SWITCH;
			};
			/^height/          && do {
				$parse{Monitor}{$count}{DisplaySize}{height} = $value[1];
				last SWITCH;
			};
			#======================================== 
			# Gamma...
			#----------------------------------------
			/^red/          && do {
				$parse{Monitor}{$count}{Gamma}{red} = $value[1];
				last SWITCH;
			};
			/^green/          && do {
				$parse{Monitor}{$count}{Gamma}{green} = $value[1];
				last SWITCH;
			};
			/^blue/          && do {
				$parse{Monitor}{$count}{Gamma}{blue} = $value[1];
				last SWITCH;
			};
			#========================================
			# HorizSync...
			#----------------------------------------
			/^Hs/          && do {
				$parse{Monitor}{$count}{HorizSync} = $value[1];
				last SWITCH;
			};
			#========================================
			# VertRefresh...
			#---------------------------------------- 
			/^Vs/          && do {
				$parse{Monitor}{$count}{VertRefresh} = $value[1];
				last SWITCH;
			};
			#========================================
			# Modeline...
			#----------------------------------------
			/^modeline/    && do {
				@list = split(/,/,$modeline);
				$mnr  = 0;
				foreach $l (@list) {
					$parse{Monitor}{$count}{Modeline}{$mnr} = $l;
					$mnr++; 
				}
				last SWITCH;
			};
			#========================================
			# Model...
			#----------------------------------------
			/^model/          && do {
				if ($i =~ /.*model=(.*)width=.*height=.*Hs=.*/) {
					$value[1] = $1;
				} elsif ($i =~ /.*model=(.*)Hs=.*/) {
					$value[1] = $1;
				}
				$parse{Monitor}{$count}{Model} = $value[1];
				last SWITCH;
			};
			#========================================
			# Option...
			# ----------
			/^option/      && do {
				@list = split(/,/,$value[1]);
				foreach $l (@list) {
					@option = split(/:/,$l);
					$parse{Monitor}{$count}{Option}{$option[0]} = $option[1];
				}
				last SWITCH;
			};
			}
			}
		}
		$count++;
		}
	}
	%parse = CleanParse (\%parse,"Monitor",$count);
	return(%parse);
}

#----[ ParseScreenSection (ptr) ]----#
sub ParseScreenSection {
#---------------------------------------------
# this function create a hash array
# containing the configuration information
# from the X11 Device sections
#
	my $ptr = $_[0];     # config pointer
	my %parse;           # result hash
	my $result;          # result string from an XFree:: call
	my @list;            # item list
	my $count;           # counter
	my $i;               # loop counter
	my $n;               # loop counter
	my $l;               # loop counter
	my $opt;             # loop counter
	my $k;               # loop counter
	my @dsplist;         # list of display subsection settings
	my @dspvalue;        # display subsection setting x:y
	my @sections;        # sections
	my @value;           # screen setting x=y
	my @optlist;         # option list in display subsection
	my @option;          # option value x:y
	my $dsp;             # display subsection counter

	$result = XFree::GetDisplaySpecs($ptr);
	if (defined $result) {
		$count = 0;
		$dsp   = 0;

		# split sections...
		# -----------------
		@sections  = split(/\|/,$result);
		foreach $i (@sections) {

			@line = split(/ /,$i);
			foreach $n (@line) {
				@value = split(/=/,$n);
				if (defined $value[0]) {
				SWITCH: for ($value[0]) {
				#========================================
				# Identifier...
				#----------------------------------------
				/^id/            && do {
					if ($value[1] =~ /.*-(.*)/) {
						$value[1] = $1;
					}
					$parse{Screen}{$count}{Identifier} = $value[1];
					last SWITCH;
				};
				#========================================
				# ObsDriver...
				#----------------------------------------
				/^obsd/          && do {
					$parse{Screen}{$count}{ObsDriver} = $value[1];
					last SWITCH;
				};
				#========================================
				# DefaultDepth...
				#----------------------------------------
				/^defaultdepth/ && do {
					$parse{Screen}{$count}{DefaultDepth} = $value[1];
					last SWITCH;
				};
				#========================================
				# DefaultBpp...
				#----------------------------------------
				/^defaultbpp/   && do {
					$parse{Screen}{$count}{DefaultBpp} = $value[1];
					last SWITCH;
				};
				#========================================
				# Device...
				#----------------------------------------
				/^device/ && do {
					if ($value[1] =~ /.*-(.*)/) {
						$value[1] = $1;
					}
					$parse{Screen}{$count}{Device} = $value[1];
					last SWITCH;
				};
				#========================================
				# Monitor...
				#----------------------------------------
				/^monitor/ && do {
					if ($value[1] =~ /.*-(.*)/) {
						$value[1] = $1;
					}
					$parse{Screen}{$count}{Monitor} = $value[1];
					last SWITCH;
				};
				#========================================
				# Option...
				#----------------------------------------
				/^option/      && do {
					@list = split(/,/,$value[1]);
					foreach $l (@list) {
						@option = split(/:/,$l);
						$parse{Screen}{$count}{Option}{$option[0]} = $option[1];
					}
					last SWITCH;
				};
				#========================================
				# Display Subsection...
				#----------------------------------------
				/^display/      && do {
					@dsplist = split(/,/,$value[1]);
					foreach $l (@dsplist) {
					@dspvalue = split(/:/,$l);
					SWITCH: for ($dspvalue[0]) {     
					#========================================
					# ViewPort...
					#----------------------------------------
					/^vpx/            && do {
						$parse{Screen}{$count}{Display}{$dsp}{ViewPort}{x} = $dspvalue[1];
						last SWITCH;
					};   
					/^vpy/            && do {
						$parse{Screen}{$count}{Display}{$dsp}{ViewPort}{y} = $dspvalue[1];
						last SWITCH;
					};      
					#========================================
					# Virtual...
					#----------------------------------------
					/^vx/            && do {
						$parse{Screen}{$count}{Display}{$dsp}{Virtual}{x} = $dspvalue[1];
						last SWITCH;
					};
					/^vy/            && do {
						$parse{Screen}{$count}{Display}{$dsp}{Virtual}{y} = $dspvalue[1];
						last SWITCH;
					};
					#========================================
					# Depth...
					#----------------------------------------
					/^depth/         && do {
						$parse{Screen}{$count}{Display}{$dsp}{Depth} = $dspvalue[1];
						last SWITCH;
					};
					#========================================     
					# Bpp...
					#----------------------------------------
					/^bpp/           && do {
						$parse{Screen}{$count}{Display}{$dsp}{Bpp} = $dspvalue[1];
						last SWITCH;
					};
					#========================================
					# Visual...
					#----------------------------------------
					/^visual/        && do {
						$parse{Screen}{$count}{Display}{$dsp}{Visual} = $dspvalue[1];
						last SWITCH;
					};     
   					#========================================
					# Weight...
					#----------------------------------------
					/^w.red/         && do {
						$parse{Screen}{$count}{Display}{$dsp}{Weight}{red} = $dspvalue[1];
						last SWITCH;
					};
					/^w.green/       && do {
						$parse{Screen}{$count}{Display}{$dsp}{Weight}{green} = $dspvalue[1];
						last SWITCH;
					};
					/^w.blue/        && do {
						$parse{Screen}{$count}{Display}{$dsp}{Weight}{blue} = $dspvalue[1];
						last SWITCH;
					};
					#========================================
					# Black...
					#----------------------------------------
					/^bt.red/         && do {
						$parse{Screen}{$count}{Display}{$dsp}{Black}{red} = $dspvalue[1];
						last SWITCH;
					};
					/^bt.green/      && do {
						$parse{Screen}{$count}{Display}{$dsp}{Black}{green} = $dspvalue[1];
						last SWITCH;
					};
					/^bt.blue/       && do {
						$parse{Screen}{$count}{Display}{$dsp}{Black}{blue} = $dspvalue[1];
						last SWITCH;
					};
					#========================================
					# White...
					#----------------------------------------
					/^wt.red/         && do {
						$parse{Screen}{$count}{Display}{$dsp}{White}{red} = $dspvalue[1];
						last SWITCH;
					};
					/^wt.green/      && do {
						$parse{Screen}{$count}{Display}{$dsp}{White}{green} = $dspvalue[1];
						last SWITCH;
					};
					/^wt.blue/       && do {
						$parse{Screen}{$count}{Display}{$dsp}{White}{blue} = $dspvalue[1];
						last SWITCH;
					};
					#========================================
					# Modes...
					#----------------------------------------
					/^modes/         && do {
						$parse{Screen}{$count}{Display}{$dsp}{Modes} = $dspvalue[1];
						last SWITCH;
					};
					#========================================
					# Option...
					# ----------
					/^option/        && do {
						$opt = $value[2];
						$opt =~ s/optname://g;
						$opt =~ s/optval://g;
						@optlist = split(/,/,$opt);
						for ($k=0;$k <@optlist; $k+=2) {
							$option[0] = $optlist[$k];
							$option[1] = $optlist[$k + 1];
							if ($option[1] =~ /none/i) {
								$option[1] = "";
							}
							$parse{Screen}{$count}{Display}{$dsp}{Option}{$option[0]}=$option[1];
						}
						last SWITCH;
					};
					}
					}
					$dsp++;
					last SWITCH;
				};
				}
				}
			}
		$count++;
		}
	}
	%parse = CleanParse (\%parse,"Screen",$count);
	return(%parse);
}

1;
