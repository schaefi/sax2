# /.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2001
#
# CreateSections.pm configuration level 2
# Create the Extensions section of the xorg.conf
#
# CVS ID:
# --------
# Status: Up-to-date
#
#---[ CreateExtensionsSection ]----#
sub CreateExtensionsSection {
#----------------------------------------------
# create the ServerFlags section...
#
	my (%var)       = %{$_[0]};  # configuration hash (argument)  
	my $setting;                 # member of hash element
	my @result;                  # section result list
	my $size;                    # sizeof list @list
	my @list;                    # generic list
	my $i;                       # loop counter
	my $n;                       # loop counter

	# create header line...
	@result = ();
	push(@result,"Section \"Extensions\"\n");

	# create body...
	foreach $i (sort keys %{$var{Extensions}{0}}) {
		$setting = $var{Extensions}{0}{$i};
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
			# Option setting...
			#-------------------------------------------
			/^Option/    && do {
				foreach $n (@list) {
				if ($n ne "") {
					push(@result,PrintLine($i,"\"$n\""));
				}
				}
				last SWITCH;
			};
			#===========================================
			# default settings
			#-------------------------------------------
			foreach $n (@list) {
			if (($i ne "") && ($n ne "")) {
				push(@result,PrintLine("Option","\"$i\" \"$n\""));
			}
			}
			}
		}
	}
	# create footer line...
	push(@result,"EndSection\n");
	return(@result);
}

1;
