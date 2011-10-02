# /.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2001
#
# CreateSections.pm configuration level 2
# Create the Device section of the xorg.conf
#
# CVS ID:
# --------
# Status: Up-to-date
#

#---[ CreateDeviceSection ]------#
sub CreateDeviceSection {
#---------------------------------------------
# create the device section...
#
	my (%var)  = %{$_[0]};  # configuration hash (argument)  
	my $setting;            # member of hash element
	my @result;             # section result list
	my $size;               # sizeof list @list
	my @list;               # generic list
	my $i;                  # loop counter
	my $n;                  # loop counter
	my $l;                  # loop counter
	my $count;              # loop counter
	my $section;            # section number

	@result = ();
	foreach $section (sort keys %{$var{Device}}) {
	push(@result,"Section \"Device\"\n");

	# create body...
	foreach $i (sort keys %{$var{Device}{$section}}) {
		$setting = $var{Device}{$section}{$i};
		$setting = prepare_setting ($setting);
		if ($setting eq "") { 
			next; 
		}
		if ($i !~ /^VendorName|BoardName/) {
			@list = split(/,/,$setting);
		} else {
			@list = ();
			$list[0] = $setting;
		}
		$size = @list;
		SWITCH: for ($i) {
		#===========================================
		# Clocks setting...
		#------------------------------------------- 
		/^Clocks/                   && do {
			@list = split(/:/,$setting);
			foreach $n (@list) {
			if ($n ne "") {
				push(@result,PrintLine("Option","\"$i\" \"$n\""));
			}
			}
			last SWITCH;
		};
		#===========================================
		# Videoram setting...
		#------------------------------------------- 
		/^Videoram|VideoRam|Screen/ && do {
			foreach $n (@list) {
			if ($n ne "") {
				$n =~ s/\"//g;
				push(@result,PrintLine($i,$n));
			}
			}
			last SWITCH;
		};
		#===========================================
		# BusID setting...
		#-------------------------------------------
		/^BusID/i                   && do {
			if ($setting ne "Single") {
				push(@result,PrintLine($i,"\"$setting\""));
			}
			last SWITCH;
		};
		#===========================================
		# ID settings...
		#-------------------------------------------
		/^ChipID|IOBase|ChipRev|TextClockFreq|MemBase|BiosBase/i && do {
			foreach $n (@list) {
			if ($n ne "") {
				$n =~ s/\"//g;
				push(@result,PrintLine($i,$n));
			}
			}
			last SWITCH;
		};
		#===========================================
		# Raw...
		#-------------------------------------------
		/^Raw/                      && do {
			foreach $count (sort keys %{$var{Device}{$section}{$i}}) {
			foreach $n (sort keys %{$var{Device}{$section}{$i}{$count}}) {
				$setting = $var{Device}{$section}{$i}{$count}{$n};
				if ($setting ne "") {
					@list = split(/,/,$setting);
					@list = CheckSplit (@list);
					$size = @list;
					foreach $l (@list) {
						$l =~ s/^ +//;
						push(@result,PrintLine($n,$l));
					}
				}
			}
			} 
			last SWITCH;
		};
		#============================================================
		# Special setting ( options which need on/off statement)...
		#------------------------------------------------------------
		/^Special/                  && do { 
			foreach $n (sort keys %{$var{Device}{$section}{$i}}) {
			$setting = $var{Device}{$section}{$i}{$n};
			if ($setting ne "") {
				$setting = prepare_setting ($setting);
				@list = split(/,/,$setting);
				$size = @list;
				foreach $l (@list) {
				if ($l eq "unset") {
					push(@result,PrintLine("Option","\"$n\" \"off\""));
				} elsif ($l eq "set") {
					push(@result,PrintLine("Option","\"$n\" \"on\""));
				} else {
					push(@result,PrintLine("Option","\"$n\" \"$l\""));
				}
				}
			}
			}
			last SWITCH;
		};
		#===========================================
		# default setting...
		#------------------------------------------- 
		foreach $n (@list) {
		if ($n ne "") {
			push(@result,PrintLine($i,"\"$n\""));
		}
		}
	}
	}
	push(@result,"EndSection\n\n");
	} 
	return(@result);
}


#---[ prepare_setting ]---#
sub prepare_setting {
#-----------------------------------------
# prepare xorg.conf setting....
#
	my $setting = $_[0];
	$setting =~ s/^ +//g; # remove blanks
	$setting =~ s/\t+//g; # remove tabs
	$setting =~ s/\"//g;  # remove " signs
	$setting =~ s/^ +//g; # remove rest blanks
	$setting =~ s/\n//g;  # remove returns
	return($setting);
}

#---[ CheckSplit ]----#
sub CheckSplit {
#------------------------------------------------------
# check if a splitted list with seperator [,] was
# splitted in a correct way. This function will check
# for the number ["] signs and make sure there was
# no split between optionslist which mustn't splitted
#
	my @list   = @_;
	my @result = ();
	my $count  = 0;
	my $joined;
	for (my $i=0;$i<@list;$i++) {
		my $signs = countDoubleQuote ($list[$i]);
		if ( ($signs == 0) || ($signs % 2 != 0) ) {
			$joined.=",$list[$i]";
		} else {
			$result[$count] = $list[$i];
			$count++;
			if ($joined ne "") {
				$joined =~ s/^,//;
				$result[$count] = $joined;
				$joined = "";
				$count++;
			}
		}
	}
	if ($joined ne "") {
		$joined =~ s/^,//;
		push (@result,$joined);
	}
	@list   = @result;
	@result = ();
	foreach (@list) {
	if (defined $_) {
		push (@result,$_);
	}
	}
	return @result;
}

#---[ countDoubleQuote ]----#
sub countDoubleQuote {
#---------------------------------------
# count double quotes in a string and
# return the result
#
	my $str = $_[0];
	my $cnt = 0;
	while (length ($str)) {
		if (chop($str) eq "\"") {
			$cnt++;
		}
	}
	return $cnt;
}

1;
