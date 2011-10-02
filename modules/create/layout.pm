# /.../
# Copyright (c) 2001 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2001
#
# CreateSections.pm configuration level 2
# Create the ServerLayout section of the xorg.conf
#
# CVS ID:
# --------
# Status: Up-to-date
#
use strict;

#---[ CreateServerLayoutSection ]----#
sub CreateServerLayoutSection {
#-------------------------------------------------
# create the server layout section and check
# the server layout again. This is the only
# section which has some sort of intelligence :-)
#
	my (%var)  = %{$_[0]};  # configuration hash (argument)  
	my @result = ();

	foreach my $lid (sort keys %{$var{ServerLayout}}) {
		if ($lid eq "") {
			next;
		}
		my $clone = $var{ServerLayout}{$lid}{Option}{Clone};
		my $ident = $var{ServerLayout}{$lid}{Identifier};
		push (@result,"Section \"ServerLayout\"\n");
		push (@result,PrintLine("Identifier","\"$ident\""));
		#===========================================
		# create body...
		#-------------------------------------------
		foreach my $i (sort keys %{$var{ServerLayout}{$lid}}) {
		SWITCH: for ($i) {
		#===========================================
		# Screen...
		#------------------------------------------- 
		/^Screen/    && do {
			# /.../
			# Left  assigned to number: 1
			# Right assigned to number: -1
			# Above assigned to number: 2
			# Below assigned to number: -2
			# ---
			my $single = 0;
			my @configList;
			my @indexList;
			my @screenList;
			foreach my $scrnr (sort keys %{$var{ServerLayout}{$lid}{$i}}) {
				my $id    = $var{ServerLayout}{$lid}{$i}{$scrnr}{id};
				my $top   = $var{ServerLayout}{$lid}{$i}{$scrnr}{top};
				my $bot   = $var{ServerLayout}{$lid}{$i}{$scrnr}{bottom};
				my $left  = $var{ServerLayout}{$lid}{$i}{$scrnr}{left};
				my $right = $var{ServerLayout}{$lid}{$i}{$scrnr}{right};
				my $rel   = $var{ServerLayout}{$lid}{$i}{$scrnr}{relative};
				my @ID;
				push (@screenList,$id);
				if ($id  =~ /Screen\[(.*)\]/) {
					$ID[0] = $1;
				}
				my @list = ($left,$right,$top,$bot);
				for (my $n=0;$n<@list;$n++) {
				if ($list[$n] =~ /Screen\[(.*)\]/) {
					$ID[$n+1] = $1;
				}
				}
				if ($id ne "") {
				# new Layout...
				# -------------
				if ($top ne "") {
					push(@configList,"  $i       \"$id\" Above \"$top\"\n");
					push(@indexList,"$ID[0]:2:$ID[3]");
				}
				if ($bot ne "") { 
					push(@configList,"  $i       \"$id\" Below \"$bot\"\n");
					push(@indexList,"$ID[0]:-2:$ID[4]");
				}
				if ($left ne "") {
					push(@configList,"  $i       \"$id\" LeftOf \"$left\"\n");
					push(@indexList,"$ID[0]:1:$ID[1]");
				}
				if ($right ne "") {
					push(@configList,"  $i       \"$id\" RightOf \"$right\"\n");
					push(@indexList,"$ID[0]:-1:$ID[2]");
				}
				if (
					($top  eq "") && ($bot   eq "") && 
					($left eq "") && ($right eq "") 
				) {
					push(@configList,"  $i       \"$id\"\n");
					$single = 1;
				}
				if (($rel ne "") && ($rel =~ /(\d+)-(\d+)-(.*)/)) {
					my $x = $1;
					my $y = $2;
					my $s = $3;
					my $rsetup = "\"$id\" Relative \"$s\" $x $y";
					push(@result,"  Screen       $rsetup\n");
				}
				}
			}
			# check for redundant layout...
			# -------------------------------
			if (! $single) {
				my @finalList;
				for (my $i=0;$i<@indexList;$i++) {
					my @item = split(/:/,$indexList[$i]);
					my $a = $item[2];
					my $c = $item[0];
					my $b = $item[1] * -1;
					my $search = "$a:$b:$c";
					my $found  = 0;
					for (my $n=0;$n<@indexList;$n++) {
					if ($indexList[$n] eq $search) {
						push (@finalList,$configList[$n]);
						$indexList[$i] = "";
						$indexList[$n] = "";
						$found = 1;
						last;
					}
					}
					if ((! $found ) && ($indexList[$i] ne "")) {
						push (@finalList,$configList[$i]);
					}
				}
				@configList = @finalList;
			}
			foreach (@configList) {
			if ($_ =~ /(Screen\[.*\]).*\".*\"/) {
				my $currentScreen = $1;
				if ($clone eq "on") {
					push(@result,"  Screen       \"$currentScreen\"\n");
				} else {
					push (@result,$_);
				}
				for (my $n=0;$n<@screenList;$n++) {
				if ($screenList[$n] eq $currentScreen) {
					delete $screenList[$n];
					last;
				}
				}
			}
			}
			@screenList = sortList (@screenList);
			foreach (@screenList) {
				push(@result,"  Screen       \"$_\"\n");
			}
			last SWITCH;
		};
		#===========================================
		# InputDevice setting...
		#-------------------------------------------
		/^InputDevice/    && do {
			foreach my $devnr (sort keys %{$var{ServerLayout}{$lid}{$i}}) {
				my $input_id  = $var{ServerLayout}{$lid}{$i}{$devnr}{id};
				my $input_use = $var{ServerLayout}{$lid}{$i}{$devnr}{usage};
	  
				if ($input_id ne "") {
				if ($input_use ne "none") {
					push(@result,PrintLine($i,"\"$input_id\" \"$input_use\""));
				} else {
					push(@result,PrintLine($i,"\"$input_id\""));
				}
				}
			}
			last SWITCH;
		};   
		#===========================================
		# Option setting...
		#-------------------------------------------
		/^Option/         && do {
			foreach my $optname (sort keys %{$var{ServerLayout}{$lid}{$i}}) {
				my $value = $var{ServerLayout}{$lid}{$i}{$optname};
				if ($value ne "") {
					push(@result,PrintLine("Option","\"$optname\" \"$value\""));
				}
			}
			last SWITCH;
		};
		}
		}
		# create footer line...
		push(@result,"EndSection\n\n");
	}
	return(@result);
}

#---[ sortList ]---#
sub sortList {
#-------------------------------------------------------
# create sorted list according to the screen
# numbers in the server layout specifications
#
	my @list = @_;
	my @index;
	my @tmp;
	my @erg;

	foreach (@list) {
		if (! defined $_) {
			next;
		}
		if ($_ =~ /Screen\[(.*)\]/) {
			if (! defined $index[$1]) {
				push (@erg,$_);
			}
			$index[$1] = $1;
		} else {
			push (@erg,$_);
		}
	}
	return (@erg);
}

1;
