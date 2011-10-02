# /.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2001
#
# CreateSections.pm configuration level 2
# Create the Monitor section of the xorg.conf
#
# CVS ID:
# --------
# Status: Up-to-date
#

#---[ CreateMonitorSection ]----#
sub CreateMonitorSection {
#------------------------------------------------
# create the monitor section and the link name 
# to the modes section. No modelines
# will be inserted here...
#
	my (%var)       = %{$_[0]};  # configuration hash (argument)  
	my $builtin     = $_[1];     # enable/disable X Server builtin modes
	my $setting;                 # member of hash element
	my @result;                  # section result list
	my $size;                    # sizeof list @list
	my @list;                    # generic list
	my $i;                       # loop counter
	my $n;                       # loop counter
	my $l;                       # loop counter
	my $section;                 # section number
	my $mode;                    # indicate number of modeline
	my @tmp;                     # store results temporarly
	my $line;                    # one modeline
	my %modes;                   # the modes array dac->modeline
	my $dac;                     # the RamDAC value from a modeline

	@result = ();
	foreach $section (sort keys %{$var{Monitor}}) {
	push(@result,"Section \"Monitor\"\n");
	#===========================================
	# create body...
	#-------------------------------------------
	foreach $i (sort keys %{$var{Monitor}{$section}}) {
		$setting = $var{Monitor}{$section}{$i};
		if ($setting ne "") {
			$setting =~ s/^ +//g;  # remove first blanks
			$setting =~ s/\t+//g;  # remove tabs
			$setting =~ s/^\"//g;  # remove " signs
			$setting =~ s/\"$//g;  # remove " signs
			$setting =~ s/^ +//g;  # remove rest blanks
			$setting =~ s/\n//g;   # remove returns

			@list = split(/,/,$setting);
			$size = @list;

			SWITCH: for ($i) {
			#===========================================
			# catch the Modeline... 
			#-------------------------------------------
			/^Modeline/    && do {
				last SWITCH;
			};
			#===========================================
			# HorizSync setting...
			#------------------------------------------- 
			/^HorizSync/    && do {
				foreach $n (@list) {
				if (($n ne "") && ($n ne "-")) {
					push(@result,PrintLine($i,$n));
				}
				}
				last SWITCH;
			};
			#===========================================
			# VertRefresh setting...
			#------------------------------------------- 
			/^VertRefresh/    && do {
				foreach $n (@list) {
				if (($n ne "") && ($n ne "-")) {
					push(@result,PrintLine($i,$n));
				}
				}
				last SWITCH;
			};
			#===========================================
			# Display Size...
			#-------------------------------------------
			/^DisplaySize/    && do {
				foreach $n (@list) {
				if ($n ne "") {
					push(@result,PrintLine($i,$n));
				}
				}
				last SWITCH;
			};
			#===========================================
			# CalcAlgorithm...
			#-------------------------------------------
			/^CalcAlgorithm/  && do {
				foreach $n (@list) {
				if ($n ne "") {
					push(@result,PrintLine("Option","\"$i\" \"$n\""));
				}
				}
				last SWITCH;
			};
			#===========================================
			# PreferredMode...
			#-------------------------------------------
			/^PreferredMode/  && do {
				foreach $n (@list) {
				if ($n ne "") {
					push(@result,PrintLine("Option","\"$i\" \"$n\""));
				}
				}
				last SWITCH;
			};
			#===========================================
			# default setting...
			#------------------------------------------- 
			foreach $n (@list) {
			if ($n ne "") {
				$n =~ s/^\"//g;
				$n =~ s/\"$//g;
				push(@result,PrintLine($i,"\"$n\""));
			}
			}
			}
		}
	}
	# create UseModes line...
	if ($builtin ne "yes") {
		push(@result,PrintLine("UseModes","\"Modes\[$section\]\""));
	}

	# create footer line...
	push(@result,"EndSection\n\n");
	}
	return(@result);
}

1;
