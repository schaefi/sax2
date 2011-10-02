# /.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2001
#
# CreateSections.pm configuration level 2
# Create the Screen section of the xorg.conf
#
# CVS ID:
# --------
# Status: Up-to-date
#

#---[ CreateScreenSection ]----#
sub CreateScreenSection {
#----------------------------------------------
# create the screen section and the related
# subsections...
#
	my (%var)       = %{$_[0]};  # configuration hash (argument)  
	my $setting;                 # member of hash element
	my @result;                  # section result list
	my $size;                    # sizeof list @list
	my @list;                    # generic list
	my $i;                       # loop counter
	my $n;                       # loop counter
	my $l;                       # loop counter
	my $c;                       # loop counter
	my $section;                 # section number
	my $modi;                    # the Modes ... line
	my $depth;                   # list of color depths for display subsection
	my $subset;                  # settings within display subsection

	@result = ();
	foreach $section (sort keys %{$var{Screen}}) {
	push(@result,"Section \"Screen\"\n");
	#===========================================
	# create body...
	#-------------------------------------------
	foreach $i (sort keys %{$var{Screen}{$section}}) {
		$setting = $var{Screen}{$section}{$i};
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
			# Display subsections...
			#-------------------------------------------
			/^Depth/    && do {
				foreach $depth (sort keys %{$var{Screen}{$section}{$i}}) {
				#---[ hack ]-------#
				# 15 bit colordepth only works on SuSE version 6.3 or
				# higher. Therefore it is disabled here
				# if ($depth == 15) { next; } 
				# ....
	  
				if ($var{Screen}{$section}{$i}{$depth}{Modes} eq "") {    
					next;
				}   
				push(@result,"  SubSection \"Display\"\n");
				push(@result,PrintLine("  $i",$depth));
				foreach $subset (sort keys %{$var{Screen}{$section}{$i}{$depth}}) {
					$setting = $var{Screen}{$section}{$i}{$depth}{$subset};
					if ($setting ne "") {
						$setting =~ s/^ +//g; # remove blanks
						$setting =~ s/\t+//g; # remove tabs
						$setting =~ s/\"//g;  # remove " signs
						$setting =~ s/^ +//g; # remove rest blanks
						$setting =~ s/\n//g;  # remove returns
	 
						@list = split(/,/,$setting);
						$size = @list;
	
						SWITCH: for ($subset) {
						#===========================================
						# Modes line...
						#-------------------------------------------
						/^Modes/    && do {
							$modi = "";
							foreach $l (@list) {
								$modi = "$modi\"$l\" ";
							} 
							if ($modi ne "") {
								push(@result,PrintLine("  $subset",$modi));
							}
							last SWITCH;
						};       
						#===========================================
						# Viewport setting...
						#------------------------------------------- 
						/^ViewPort/    && do {
							foreach $l (@list) {
							if ($l ne "") {
								push(@result,PrintLine("  $subset",$l));
							}
							} 
							last SWITCH;
						}; 
						#===========================================
						# Virtual setting...
						#-------------------------------------------
						/^Virtual/    && do {
							foreach $l (@list) {
							if ($l ne "") {
								push(@result,PrintLine("  $subset",$l));
							}
							} 
							last SWITCH;
						}; 
						#===========================================
						# Weight tok...
						#-------------------------------------------
						/^Weight/    && do {
							foreach $l (@list) {
							if ($l ne "") {
								push(@result,PrintLine("  $subset",$l));
							}
							}
							last SWITCH;
						};
						#===========================================
						# Black tok...
						#-------------------------------------------
						/^Black/    && do {
							foreach $l (@list) {
							if ($l ne "") {
								push(@result,PrintLine("  $subset",$l));
							}
							}
							last SWITCH;
						};
						#===========================================
						# White tok...
						#-------------------------------------------
						/^White/    && do {
							foreach $l (@list) {
							if ($l ne "") {
								push(@result,PrintLine("  $subset",$l));
							}
							}
							last SWITCH;
						};
						#===========================================
						# Raw...
						#-------------------------------------------
						/^Raw/      && do {
							foreach $c (sort keys %{$var{Screen}{$section}{$i}{$depth}{$subset}}) {
							foreach $n (sort keys %{$var{Screen}{$section}{$i}{$depth}{$subset}{$c}}) {
								$setting = $var{Screen}{$section}{$i}{$depth}{$subset}{$c}{$n};
								if ($setting ne "") {
									@list = split(/,/,$setting);
									$size = @list;
									foreach $l (@list) {
										push(@result,PrintLine("  $n",$l));
									} 
								}
							}
							}
							last SWITCH;
						};
						#==========================================================
						# Special setting ( options which need on/off statement)...
						# ---------------------------------------------------------
						/^Special/    && do {
							foreach $n (sort keys %{$var{Screen}{$section}{$i}{$depth}{$subset}}) {
							$setting = $var{Screen}{$section}{$i}{$depth}{$subset}{$n};
							if ($setting ne "") {
								$setting =~ s/^ +//g; # remove blanks
								$setting =~ s/\t+//g; # remove tabs
								$setting =~ s/\"//g;  # remove " signs
								$setting =~ s/^ +//g; # remove rest blanks
								$setting =~ s/\n//g;  # remove returns

								@list = split(/,/,$setting);
								$size = @list;

								foreach $l (@list) {
								if ($l eq "unset") {
									push(@result,PrintLine("  Option","\"$n\" \"off\"")); 
								} elsif ($l eq "set") {
									push(@result,PrintLine("  Option","\"$n\" \"on\""));
								} else {
									push(@result,PrintLine("  Option","\"$n\" \"$l\""));
								}
								}
							}
							}
							last SWITCH;
						};
						#===========================================
						# default (include normal options)... 
						#-------------------------------------------
						foreach $l (@list) {
						if ($l ne "") {
							push(@result,PrintLine("  $subset","\"$l\""));
						}
						}
						} 
					}
				}
				push(@result,"  EndSubSection\n");
				}
				last SWITCH;
			};    
			#===========================================
			# DefaultDepth setting...
			#-------------------------------------------
			/^DefaultDepth|DefaultFbBpp/    && do {
				foreach $n (@list) {
				if ($n ne "") {
					push(@result,PrintLine($i,$n));
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
	}
	# create footer line...
	push(@result,"EndSection\n\n");
	}
	return(@result);
}

1;
