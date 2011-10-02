#/.../
# Copyright (c) 2000 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
# HashMap.pm:  
#  - set and get hash information via command strings
#  - reset ServerLayout if needed
#
# CVS ID:
# --------
# Status: Up-to-date
#

#----[ HSetValue ]---------#
sub HSetValue {
#----------------------------------------------------------
# this function is used to add some data to the given
# hash, returning the new hash value
#
	my (%var)   = %{$_[0]};   # the hash
	my ($str)   = $_[1];      # the hash path
	my ($val)   = $_[2];      # the value for the hash
 
	my @keys;                 # single hash keys
	my $eval;                 # evaluation string

	@keys = split(/->/, $str);
	$eval = "\$var";
	foreach (@keys) {
		$eval .= "{'$_'}";
	}
	$eval .= " = '$val'";
	eval($eval);
	return(%var);
}

#----[ HRemoveValue ]-------#
sub HRemoveValue {
#----------------------------------------------------------
# this function is used to remove data from a given
# hash
#
	my (%var) = %{$_[0]};     # the hash
	my ($str) = $_[1];        # the hash path to remove

	my @keys;                 # single hash keys
	my $eval;                 # evaluation string

	@keys = split(/->/, $str);
	$eval = "delete \$var";
	foreach (@keys) {
		$eval .= "{'$_'}";
	}
	eval ($eval);
	return (%var);
}

#----[ HMoveBranch ]--------#
sub HMoveBranch {
#----------------------------------------------------------
# this function moves the branch H-><name><NUMBER-1> to
# H-><name><NUMBER-2> This movement only take effect on
# the sections: Device Screen and Monitor
#
	my (%var)       = %{$_[0]}; # the hash
	my ($oldbranch) = $_[1];    # old number of the branch

	my $main;                   # main branch key
	my $branch;                 # number of main branch to move
	my $newbranch;              # new branch number

	sub inverse { $b <=> $a; };
	if ($oldbranch !~ /[0-9]+/) {
		return(%var);
	}
	foreach $main (keys %var) {
	foreach $branch (sort inverse keys %{$var{$main}}) {
		if ($branch !~ /[0-9]+/) {
			next;
		}
		if ($branch >= $oldbranch) {
		if (
			($main eq "Device")      || 
			($main eq "Screen")      ||
			($main eq "Monitor")     ||
			($main eq "InputDevice")
		) {
			if ($main eq "InputDevice") {
				$newbranch = $branch + 2;
			} else {
				$newbranch = $branch + 1;
			}
			$var{$main}{$newbranch} = $var{$main}{$branch};
    
			# adapt Identifier,Device,Monitor 
			# -------------------------------------------
			if (defined $var{$main}{$newbranch}{Identifier}) {
				$value = $var{$main}{$newbranch}{Identifier};
				$value =~ s/\[.*\]/\[$newbranch\]/;
				$var{$main}{$newbranch}{Identifier} = $value;
			}
			if (defined $var{$main}{$newbranch}{Device}) {
				$value = $var{$main}{$newbranch}{Device};
				$value =~ s/\[.*\]/\[$newbranch\]/;
				$var{$main}{$newbranch}{Device}     = $value;
			}
			if (defined $var{$main}{$newbranch}{Monitor}) {
				$value = $var{$main}{$newbranch}{Monitor};
				$value =~ s/\[.*\]/\[$newbranch\]/;
				$var{$main}{$newbranch}{Monitor}    = $value;
			}
			if ($main eq "InputDevice") {
				$value = $var{ServerLayout}{all}{$main}{$branch}{id};
				$value =~ s/\[.*\]/\[$newbranch\]/;
				$var{ServerLayout}{all}{$main}{$newbranch}{id} = $value;
				$value = $var{ServerLayout}{all}{$main}{$branch}{usage};
				$var{ServerLayout}{all}{$main}{$newbranch}{usage} = $value;
				delete $var{ServerLayout}{all}{$main}{$branch};
			}

			# delete old branch...
			# ---------------------
			if ($branch == $oldbranch) {
				delete($var{$main}{$branch});
			}
		}
		}
	}
	}
	return(%var);
}

#----[ HGetValue ]---------#
sub HGetValue {
#----------------------------------------------------------
# this function will traverse the hash with a recursive
# inline function to obtain either the hash value or
# the hash reference at the required point
#
	my (%var)    = %{$_[0]};  # the hash
	my ($use)    = $_[2];     # return reference or value

	local ($str) = $_[1];     # the search keys
	my $k;                    # key
	my $v;                    # value 
	my $ref;                  # reference
	my $a;                    # joined keys

	local $ViewReference;     # founded reference
	local $ViewValue;         # founded value
	local @key;               # key list

	if (! defined $use) {
		$use = "GET_VALUE";
	}
	$ViewReference = "";
	$ViewValue     = "";
	$ref           = \%var;

	while(($k,$v) = each(%var)) {
	@key = ();
	push(@key,$k);
	if (ref($v) eq "HASH") {
		$a = join("->",@key);
		if ($a eq $str) {
			$ViewReference = $v;
		}
		$ref = $var{$k};
		ViewBranch($ref,$str);
	} else {
		$a = join("->",@key);
		if ($a eq $str) {
			$ViewReference = $ref;
			$ViewValue = $v;
		}
	}
	}
	# return value or reference...
	# -----------------------------
	if ($use eq "GET_REF") {
		return($ViewReference);
	}
	return($ViewValue);

	# Inline sub routine for recursive call
	# --------------------------------------
	sub ViewBranch {
		my $ref = $_[0];
		my $k;
		my $v;
		my $a;

		while(($k,$v) = each(%{$ref})) {
		push(@key,$k);
		if (ref($v) eq "HASH") {
			$a = join("->",@key);
			if ($a eq $str) {
				$ViewReference = $v;
			}
			ViewBranch($v,$str);
		} else {
			$a = join("->",@key);
			if ($a eq $str) {
				$ViewValue = $v;
			}
		}
		pop(@key);
		}
		return;
	}
}


#----[ HGet ]---------#
sub HGet {
#----------------------------------------------------------
# get a complete hash and return a list of the 
# result like "key->key->...->key = value" strings
#
	my (%var)   = %{$_[0]};   # the hash

	my $k;                    # key
	my $v;                    # value 
	my $ref;                  # reference
	my $a;                    # joined keys

	local @key;               # key list
	local @tree;              # result tree

	while(($k,$v) = each(%var)) {
		@key = ();
		push(@key,$k);
		if (ref($v) eq "HASH") {
			$ref = $var{$k};
			TreeBranch($ref);
		} else {
		$a = join("->",@key);
		push(@tree,"$a = $v"); 
		}
	}
	return(@tree);

	# Inline sub routine for recursive call
	# --------------------------------------
	sub TreeBranch {
		my $ref  = $_[0];
		my $k;
		my $v;
		my $a;

		while(($k,$v) = each(%{$ref})) {
			push(@key,$k);
			if (ref($v) eq "HASH") {
				TreeBranch($v);
			} else {
				$a = join("->",@key);
				push(@tree,"$a = $v");
			}
			pop(@key);
		}
		return;
	}
}

#----[ HDif ]-------------#
sub HDif {
#----------------------------------------------------------
# lookup the settings which are in the new hash but
# not in the old one...
#
	my (%old) = %{$_[0]};
	my (%new) = %{$_[1]};

	my $k;      # key
	my $v;      # value 
	my $ref;    # reference
	my $a;      # joined keys
	my %result; # result hash

	local @key; # key list

	while(($k,$v) = each(%new)) {
		@key = ();
		push(@key,$k);
		if (ref($v) eq "HASH") {
			$ref = $new{$k};
			TreeBranchDif($ref);
		} else {
			$a = join("->",@key);
			if (HGetValue (\%old,$a) ne $v) {
				%result = HSetValue (\%result,$a,$v);
			}
		}
	}
	return(%result);

	# Inline sub routine for recursive call
	# --------------------------------------
	sub TreeBranchDif {
		my $ref  = $_[0];
		my $k;
		my $v;
		my $a;

		while(($k,$v) = each(%{$ref})) {
			push(@key,$k);
			if (ref($v) eq "HASH") {
				TreeBranchDif($v);
			} else {
				$a = join("->",@key);
				if (HGetValue (\%old,$a) ne $v) {
					%result = HSetValue (\%result,$a,$v);
				}
			}
			pop(@key);
		}
		return;
	}
}

#----[ HUpdateServerLayout ]-----#
sub HUpdateServerLayout {
#--------------------------------------------------------
# this function is called to update the server layout 
# according to changes made on the hash I know this is 
# not the expected place for this function... 
#
	my (%var)      = %{$_[0]};             # the hash
	my ($device)   = $_[1];                # device number [optional]
	my (@dev_list) = keys %{$var{Device}}; # number list of devices
	my $size       = @dev_list;            # size of dev_list
	my $i;

	if (defined $device) {
		$size = $device;
	}

	if ($size > 1) {
	for($i=0;$i<$size;$i++) {
		$var{ServerLayout}{all}{Screen}{$i}{id}      = "Screen\[$i\]";
		$var{ServerLayout}{all}{Screen}{$i}{top}     = "<none>";
		$var{ServerLayout}{all}{Screen}{$i}{bottom}  = "<none>";

	if ($i == 0) {
		$scr = $i + 1;
		$var{ServerLayout}{all}{Screen}{$i}{right}  = "Screen\[$scr\]";
		$var{ServerLayout}{all}{Screen}{$i}{left}   = "<none>";
	} elsif ($i == $size - 1) {
		$scr = $i - 1;
		$var{ServerLayout}{all}{Screen}{$i}{right}  = "<none>";
		$var{ServerLayout}{all}{Screen}{$i}{left}   = "Screen\[$scr\]";
	} else {
		$scr = $i + 1;
		$var{ServerLayout}{all}{Screen}{$i}{right}  = "Screen\[$scr\]";
		$scr = $i - 1;
		$var{ServerLayout}{all}{Screen}{$i}{left}   = "Screen\[$scr\]";
	}
	}
	}
	return(%var);
}

#----[ HPrepareServerLayout ]-----#
sub HPrepareServerLayout {
#--------------------------------------------------------
# this function is called to update the server layout
# if a profile include its own server layout which is created
# via HUpdateServerLayout(). We need this adaption to be
# sure the correct layout is written
#
	my (%var)      = %{$_[0]};             # the hash
	my (@dev_list) = keys %{$var{Device}}; # number list of devices
	my $size       = @dev_list;            # size of dev_list
	my $i;
	my $clone      = $var{ServerLayout}{all}{Option}{Clone};
	if ($size > 1) {
	for($i=0;$i<@dev_list;$i++) {
		$var{ServerLayout}{all}{Screen}{$i}{id}        = "Screen\[$i\]";
		$var{ServerLayout}{all}{Screen}{$i}{top}       = "";
		$var{ServerLayout}{all}{Screen}{$i}{bottom}    = "";
		if ($i == 0) {
		$scr = $i + 1;
		$var{ServerLayout}{all}{Screen}{$i}{right}     = "";
		if ($clone eq "on") {
			$var{ServerLayout}{all}{Screen}{$i}{left}  = "";
		} else {
		    $var{ServerLayout}{all}{Screen}{$i}{left}  = "Screen\[$scr\]";
		}
		} elsif ($i == $size - 1) {
		$scr = $i - 1;
		if ($clone eq "on") {
			$var{ServerLayout}{all}{Screen}{$i}{right} = "";
		} else {
		    $var{ServerLayout}{all}{Screen}{$i}{right} = "Screen\[$scr\]";
		}
			$var{ServerLayout}{all}{Screen}{$i}{left}  = "";
		} else {
		$scr = $i + 1;
		if ($clone eq "on") {
			$var{ServerLayout}{all}{Screen}{$i}{left}  = "";
		} else {
			$var{ServerLayout}{all}{Screen}{$i}{left}  = "Screen\[$scr\]";
		}
		}
	}
	}
	return(%var);
}

1;
