# /.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2001
#
# CreateSections.pm configuration level 2
# Create the InputDevice section of the xorg.conf
#
# CVS ID:
# --------
# Status: Up-to-date
#

#---[ CreateInputDeviceSection ]----#
sub CreateInputDeviceSection {
#----------------------------------------------
# create input device sections used for mouse
# and keyboard and special input devices
# like tablets or touchscreens...
#
	my (%var)       = %{$_[0]};  # configuration hash (argument)  
	my $setting;                 # member of hash element
	my @result;                  # section result list
	my $size;                    # sizeof list @list
	my @list;                    # generic list
	my $i;                       # loop counter
	my $n;                       # loop counter
	my $l;                       # loop counter

	#my %entity;
	#===========================================
	# merge common InputDevices...
	#-------------------------------------------
	#foreach my $section (sort keys %{$var{InputDevice}}) {
	#	my $driver = $var{InputDevice}{$section}{Driver};
	#	my $device = $var{InputDevice}{$section}{Option}{Device};
	#	if (defined $entity{$driver}{$device}) {
	#		my $pre_section = $entity{$driver}{$device};
	#		foreach my $opt (keys %{$var{InputDevice}{$section}{Option}}) {
	#			my $optval = $var{InputDevice}{$section}{Option}{$opt};
	#			$var{InputDevice}{$pre_section}{Option}{$opt} = $optval;
	#		}
	#		delete $var{InputDevice}{$section};
	#		next;
	#	}
	#	$entity{$driver}{$device} = $section;
	#}

	@result = ();
	foreach my $section (sort keys %{$var{InputDevice}}) { 
	push(@result,"Section \"InputDevice\"\n");
	#===========================================
	# create body...
	#-------------------------------------------
	foreach $i (sort keys %{$var{InputDevice}{$section}}) {
		$setting = $var{InputDevice}{$section}{$i};
		if ($setting ne "") {
			$setting =~ s/^ +//g; # remove blanks
			$setting =~ s/\t+//g; # remove tabs
			$setting =~ s/\"//g;  # remove " signs
			$setting =~ s/^ +//g; # remove rest blanks
			$setting =~ s/\n//g;  # remove returns

			@list = split(/,/,$setting);
			$size = @list;
			SWITCH: for ($i) {
			#===========================================
			# Identifier setting...
			#------------------------------------------- 
			/^Identifier/    && do {
				foreach $n (@list) {
				if ($n ne "") {
					push(@result,PrintLine($i,"\"$n\""));
				}
				}
				last SWITCH;
			};
			#===========================================
			# Driver setting...
			#-------------------------------------------
			/^Driver/    && do {
				foreach $n (@list) {
				if ($n ne "") {
					if ($n eq "kbd" || $n eq "mouse" || $n eq "vmmouse") {
						push(@result,PrintLine("# Driver \"$n\" will be disabled unless 'Option \"AutoAddDevices\" \"off\"'"));
						push(@result,PrintLine("# is set in \"ServerFlags\" section."));
					}
					push(@result,PrintLine($i,"\"$n\""));
				}
				}
				last SWITCH;
			};
			#===========================================
			# Option setting...
			#-------------------------------------------
			/^Option/    && do {
				foreach $n (sort keys %{$var{InputDevice}{$section}{$i}}) {
				$setting = $var{InputDevice}{$section}{$i}{$n};
				if ($setting ne "") {
					$setting =~ s/^ +//g; # remove blanks
					$setting =~ s/\t+//g; # remove tabs
					$setting =~ s/\"//g;  # remove " signs
					$setting =~ s/^ +//g; # remove rest blanks
					$setting =~ s/\n//g;  # remove returns

					if ($n eq "XkbVariant") {
						$setting =~ s/xfree86/basic/g;
					}

					if (
						($n ne "XkbLayout")  && ($n ne "XkbVariant") &&
						($n ne "XkbOptions")
					) {
						@list = split(/,/,$setting);
						$size = @list;
					} else {
						$setting =~ s/\!//g;
						if ($setting !~ /^,+$/) {
							@list = $setting;
						}
					}

					foreach $l (@list) {
					if ($l ne "set") {
						push(@result,PrintLine("Option","\"$n\" \"$l\""));
					} else {
						push(@result,PrintLine("Option","\"$n\""));
					}
					}
				}
				}
			};
		}
	}
	}
	# create footer line...
	push(@result,"EndSection\n\n");
	}
	return(@result);
}

1;
