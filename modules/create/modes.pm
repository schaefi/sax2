# /.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2001
#
# CreateSections.pm configuration level 2
# Create the Modes section of the xorg.conf
#
# CVS ID:
# --------
# Status: Up-to-date
#

#---[ CreateModesSection ]----#
sub CreateModesSection {
#-----------------------------------------------
# create the modes section which contains only
# the modelines belonging to the specified
# Monitor section...
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
	push(@result,"Section \"Modes\"\n");
	push(@result,PrintLine("Identifier","\"Modes\[$section\]\""));
	#===========================================
	# create body...
	#-------------------------------------------
	foreach $i (sort keys %{$var{Monitor}{$section}}) {
		$setting = $var{Monitor}{$section}{$i};
		if ($setting ne "") {
			$setting =~ s/^ +//g;  # remove first blanks
			$setting =~ s/\t+//g;  # remove tabs
			$setting =~ s/\"//g;   # remove " signs
			$setting =~ s/^ +//g;  # remove rest blanks
			$setting =~ s/\n//g;   # remove returns

			@list = split(/,/,$setting);
			$size = @list;
			SWITCH: for ($i) {
			#===========================================
			# Modeline setting...
			#------------------------------------------- 
			/^Modeline/    && do {
				if ($builtin ne "yes") {
					@tmp = ();
					sub numerisch { $a <=> $b; }
					sub daccounts { $b <=> $a; }
					foreach $mode (sort numerisch keys %{$var{Monitor}{$section}{$i}}) {
					foreach $n (keys %{$var{Monitor}{$section}{$i}{$mode}}) {
						$setting = $var{Monitor}{$section}{$i}{$mode}{$n};
						if ($setting ne "") {
							$setting =~ s/^ +//g; # remove blanks
							$setting =~ s/\t+//g; # remove tabs
							$setting =~ s/\"//g;  # remove " signs
							$setting =~ s/^ +//g; # remove rest blanks
							$setting =~ s/\n//g;  # remove returns

							@list = split(/,/,$setting);
							$size = @list;

							foreach $l (@list) {
								push(@tmp,"  $i \t\"$n\" $l\n");
							}
						}
					}
					}
					# sort the line by ramdac speeds
					# ------------------------------
					if ($var{Device}{$section}{Driver} !~ /vga/i) {
						$ldnr = 0;
						push(@tmp,"dummy\n");
						foreach $n (@tmp) {
							@line = split(/ +/,$n);
							$dac  = $line[3]; $dac =~ s/ +//g;
							$res  = $line[2]; $res =~ s/ +//g;
							# print "### $res\n";

							if (($res ne $oldres) && ($ldnr > 0)) {
								foreach $m (sort daccounts keys %modes) {
								if ($modes{$m} ne "") {
									push(@result,$modes{$m});
								}
								}
								$ldnr   = 0;
								$oldres = "";
								%modes  = ();
							}

							$ldnr++;
							$oldres = $res;
							if ($n !~ /dummy/) {
								$modes{$dac} = $n;
							}
						}
					} else {
						# /.../
						# if the vga module is used the Modelines are 
						# not sorted by ramdac speed. This is because 
						# we don't asume somebody will configure a vga
						# server, but if autodetection fails the order
						# of the given Modelines should not be changed
						# ---  
						push(@result,@tmp);
					}
				}
				last SWITCH;
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
