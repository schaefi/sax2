# /.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2001
#
# CreateSections.pm configuration level 2
# Create the Module section of the xorg.conf
#
# CVS ID:
# --------
# Status: Up-to-date
#

#---[ CreateModuleSection ]-----#
sub CreateModuleSection {
#--------------------------------------------
# create the module section and the related
# extmod subsection...
#
	my (%var)       = %{$_[0]};  # configuration hash (argument)
	my ($dlist)     = $_[1];     # do not set modules part of dlist
	my $setting;                 # member of hash element
	my @result;                  # section result list
	my $size;                    # sizeof list @list
	my @list;                    # generic list
	my $i;                       # loop counter
	my $n;                       # loop counter
	my $l;                       # loop counter
	my $empty;                   # flag to sign an empty subsection
	my $notset;                  # flag for setting module or not 
	my @critical;                # hold critical modules
	my $cm;                      # critical module name
	
	# create header line...
	@result = ();
	push(@result,"Section \"Module\"\n");

	# create body...
	foreach $i (sort keys %{$var{Module}{0}}) {
		$setting = $var{Module}{0}{$i};
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
			# Load setting...
			#-------------------------------------------
			/^Load/    && do {
				@critical = split(/,/,$dlist);
				foreach $n (@list) {
					$n =~ s/ +//g;
					$notset = 0;
					foreach $cm (@critical) {
					if ($cm eq $n) { 
						$notset = 1; last; 
					}
					}
					if (($n ne "") && ($notset == 0)) {
						$n =~ s/^ +//g; $n =~ s/ +$//g;
						push(@result,PrintLine($i,"\"$n\""));
					}
				}
				last SWITCH;
			};
			#===========================================
			# Disable setting...
			#-------------------------------------------
			/^Disable/ && do {
				@critical = split(/,/,$dlist);
				foreach $n (@list) {
					$n =~ s/ +//g;
					$notset = 0;
					foreach $cm (@critical) {
					if ($cm eq $n) {
						$notset = 1; last;
					}
					}
					if (($n ne "") && ($notset == 0)) {
						$n =~ s/^ +//g; $n =~ s/ +$//g;
						push(@result,PrintLine($i,"\"$n\""));
					}
				}
				last SWITCH;
			};
	 		#===========================================
			# Subsection extmod setting...
			#-------------------------------------------
			/^extmod/    && do {
				$empty = "true";
				push(@result,"  SubSection \"$i\"\n");

				foreach $n (sort keys %{$var{Module}{0}{$i}}) {
					$setting = $var{Module}{0}{$i}{$n};
					if ($setting ne "") {
						$empty = "false";
						$setting =~ s/^ +//g; # remove blanks
						$setting =~ s/\t+//g; # remove tabs
						$setting =~ s/\"//g;  # remove " signs
						$setting =~ s/^ +//g; # remove rest blanks
						$setting =~ s/\n//g;  # remove returns

						@list = split(/,/,$setting); 

						SWITCH: for ($n) { 
						#===========================================
						# Option....
						#-------------------------------------------
						/^Option/    && do {
							foreach $l (@list) {
								push(@result,PrintLine("  $n","\"$l\""));
							} 
							last SWITCH;
						};
						}
						push(@result,"  EndSubSection\n");
						last SWITCH;
					}
				}
				if ($empty eq "true") {
					pop(@result);
				}
			};
			}
		}
	}
	# create footer line...
	push(@result,"EndSection\n");
	return(@result);
}

1;
