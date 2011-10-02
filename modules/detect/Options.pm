#----[ AutoDetectOptions ]------#
sub AutoDetectOptions {
#----------------------------------------------------
# detecting options extensions and raw definitions
# from the sysp -q xstuff information pool
#
	my (%spec) = %{$_[0]}; # specs
	my (%var)  = %{$_[1]}; # config hash
	my %query;             # the query hash
	my @option_list;       # options
	my @extension_list;    # extensions
	my @rawdef_list;       # raw definitions
	my $size;              # number of cards
	my $i;                 # loop counter
	my $newext;            # new extension to add[+] or remove[-]
	my @rawlist;           # list of raw lines
	my $count;             # counter for rawdata entries
	my @elements;          # hold variable/value pair of rawdata in a list
	my $el;                # one raw element
	my $va;                # one raw variable

	$size  = 0;
	%query = GetHotQuery("xstuff",\%spec);
	foreach $card (sort numbers keys %query) {
	if ($card ne "") {
		foreach $i (sort keys %{$query{$card}}) {
		SWITCH: for ($i) {
			# the normal on options...
			# -------------------------
			/^Option/         && do {
			push(@option_list,$query{$card}{$i});
			last SWITCH;
			};

			# the module loadable extensions...
			# -----------------------------------
			/^Extension/      && do {
			push(@extension_list,$query{$card}{$i});
			last SWITCH;
			};

			# the raw definition lines...
			# ----------------------------
			/RawDef/          && do {
			push(@rawdef_list,$query{$card}{$i});
			last SWITCH;
			};
		}
		}
		$size++;
	}
	}
	# set detected device options...
	# --------------------------------
	for ($i=0;$i<@option_list;$i++) {
	if ($var{Device}{$i}{Driver} ne "vga") { 
		$option_list[$i] =~ s/ +//g;
		if (($option_list[$i] ne "") && ($option_list[$i] ne "None")) {
			$var{Device}{$i}{Option} = $option_list[$i];
		}
	} else {
		$var{Device}{$i}{Option} = "KGAUniversal";
	}
	}

	# set detected device raw definitions...
	# ---------------------------------------
	for($i=0;$i<@rawdef_list;$i++) {
	if (($rawdef_list[$i] ne "") && ($rawdef_list[$i] !~ /None/)) {
		@rawlist = split(/#Option |Option /,$rawdef_list[$i]);
		$count   = 0;
		foreach (@rawlist) {
			if ($_ eq "") {
				next;
			}
			$_ =~ s/,$//;
			$_ =~ s/^ +//;
			$_ =~ s/ +$//;
			@elements = split(/ +/,$_);
			$el = $elements[0];
			$va = $elements[1];
			if ($rawdef_list[$i] =~ /#Option $el $va/) {
				$var{Device}{$i}{Raw}{$count}{Option} = "#$el $va";
			} elsif ($rawdef_list[$i] =~ /Option $el $va/) {
				$var{Device}{$i}{Raw}{$count}{Option} = "$el $va";
			} else {
				$var{Device}{$i}{Raw}{$count}{$el} = $va;
			}
			$count++;
		}
	} 
	} 
 
	# set detected module extensions...
	# ----------------------------------
	$newext = join(",",@extension_list);
	$newext =~ s/None//g; $newext =~ s/,,/,/g;
	if ($newext ne "") {
		@extlist = split(/,/,$var{Module}{0}{Load});
		foreach (@extlist) {
			$ext{$_} = "on";
		}
		@extlist = split(/,/,$newext);
		foreach (@extlist) {
		if ($_ =~ /^\-/) {
			$_ =~ s/^\-//;
			$ext{$_} = "off";
		} else {
			$ext{$_} = "on";
		}
		}
		@extlist = ();
		foreach (keys %ext) {
		if ($ext{$_} eq "on") {
			push(@extlist,$_);
		}
		}
		$var{Module}{0}{Load} = join(",",@extlist);
	}
}

1;
