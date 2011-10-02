#/.../
# Copyright (c) 2005 SuSE , Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
#
# SPPParse.pm: Perl module for the SaX Profile Parser 
# provides functions to modify the registry hash via
# profile definitions
#
use lib '/usr/share/sax/modules';

use SPP;
use HashMap;
use strict;

#==========================================
# globals
#------------------------------------------
my @newSections;
my @profileData;

#==========================================
# globals
#------------------------------------------
my $indent;
my $rcount;
my $rspace;
my @rsteps;
my %rhash;
my %variables;
my $key;

#==========================================
# setup parser object
#------------------------------------------
my $parser = new SPP::SPPParse;

#==========================================
# prepareProfile
#------------------------------------------
sub prepareProfile {
	# ...
	# This function will read in the given profile and
	# prepare it for a specific device. Additionally this
	# function update the newSection list with new sections
	# ID's to get added later while the profile is applied.
	# The information is stored in the profileData() list
	# ---
	my $profile  = $_[0];
	my $card     = $_[1];
	if (! defined $card) {
		$card = 0;
	}
	my $basename = qx (basename $profile); chomp $basename;
	print "SPP: prepare device [$card] profile: $basename\n";
	if (-f "$profile.pl") {
		print "SPP: calling device [$card] profile script: $basename\n";
		qx ($profile.pl $card);
		$profile = "/var/lib/sax/$basename.tmp";
	}
	open (FD,$profile) ||
		die "SPP: could not open file: $profile";
	while (my $line = <FD>) {
		chomp ($line);
		if ($line =~ /^#/) {
			next;
		}
		SWITCH: for ($line) {
			#==========================================
			# Replace [X] with $card
			#------------------------------------------
			/^.*\[X\].*/         && do {
				$line =~ s/\[X\]/$card/g;
			};
			#==========================================
			# Replace [X+n] with $card + n
			#------------------------------------------
			/^.*\[X\+([1-9])\].*/  && do {
				my $new = $card + $1;
				$line =~ s/\[X\+[1-9]\]/$new/g;
				push (@newSections,$new);
			};
		}
		push (@profileData,$line);
	}
	close FD;
}

#==========================================
# addProfileData
#------------------------------------------
sub addProfileData {
	# ...
	# This function is used to enhance the profileData
	# list with one additional line
	# ---
	my $line = $_[0];
	push (@profileData,$line);
}

#==========================================
# prepareNewSections
#------------------------------------------
sub prepareNewSections {
	# ...
	# if a profile includes new sections we have to create 
	# space for this sections. Therefore the branch on the
	# old section is moved to the next section ( old +1 )
	# ---
	my %data = %{$_[0]};
	@newSections = unique (@newSections);
	foreach my $newsection (@newSections) {
		print "SPP: profile will add new section(s) -> moving: $newsection\n";
		%data = HMoveBranch (\%data,$newsection);
	}
	return %data;
}

#==========================================
# includeProfile
#------------------------------------------
sub includeProfile {
	# ...
	# this function will include the profile information in
	# @profileData into the given data dictionary -> change it
	# and return a new data dictionary
	# ---
	my %data = %{$_[0]};
	my $info = $_[1];
	if (@profileData == 0) {
		#==========================================
		# no profile information use shell
		#------------------------------------------
		while ($parser->parse()) {
			%data = handleAction (\%data,$parser->getAction());
		}
	} else {
		#==========================================
		# include data from @profileData
		#------------------------------------------
		%data = prepareNewSections (\%data);
		print "SPP: including prepared profile(s)...\n";
		foreach (@profileData) {
		if ($parser->parseLine ($_)) {
			%data = handleAction (\%data,$parser->getAction(),$info);
		}
		}
	}
	%data = HPrepareServerLayout (\%data);
	return %data;
}

#==========================================
# handleAction
#------------------------------------------
sub handleAction {
	# ...
	# This function will handle the result from the parser
	# error's have been caught from the parser so we don't have
	# to take care about syntax problems here
	# ---
	my %data  = %{$_[0]};
	my @parse = @{$_[1]};
	my %info  = ();
	if (defined $_[2]) {
		%info  = %{$_[2]};
	} else {
		undef %info;
	}
	#==========================================
	# update variable hash
	#------------------------------------------
	if (%info) {
		updateVariableHash ( \%info );
	} else {
		updateVariableHash ( \%data );
	}

	#==========================================
	# check if action is needed...
	#------------------------------------------
	if (@parse == 0) {
		return %data;
	}
	my $keytag = $parse[0];
	my $action = $parse[1];

	#==========================================
	# read entry and print the result
	#------------------------------------------
	if ($action eq "__read__") {
		$indent = 0;
		$rcount = 0;
		$rspace = 0;
		@rsteps = ();
		%rhash  = ();
		$key    = "";
		actionReadTree ( \%data,$keytag );
		return %data;
	}
	#==========================================
	# got a command like !help, etc...
	#------------------------------------------
	if ($action eq "__cmd__") {
		actionDoCommand ( $keytag );
		return %data;
	}
	#==========================================
	# want to remove a subtree
	#------------------------------------------
	if ($keytag eq "!remove") {
		%data = actionDoRemove ( \%data,$action );
		return %data;
	}
	#==========================================
	# handle special value keys
	#------------------------------------------
	if ($action =~ /__VESA__/) {
		$action =~ s/__VESA__/--> VESA/g;
	}
	if ($action =~ /__EQUALS__/) {
		$action =~ s/__EQUALS__/=/g;
	}
	if ($action =~ /__OB__/) {
		$action =~ s/__OB__/(/g;
	}
	if ($action =~ /__CB__/) {
		$action =~ s/__CB__/)/g;
	}
	if ($action =~ /__AND__/) {
		$action =~ s/__AND__/&/g;
	}
	if ($action =~ /__PLUS__/) {
		$action =~ s/__PLUS__/\+/g;
	}
	if ($action =~ /__GLOB__/) {
		$action =~ s/__GLOB__/\*/g;
	}
	#==========================================
	# want to set a value
	#------------------------------------------
	%data = actionDoSetValue ( \%data,$keytag,$action );
	return %data;
}

#==========================================
# actionDoSetValue
#------------------------------------------
sub actionDoSetValue {
	# ...
	# This function will set a value to the given
	# data dictionary and return the changed version
	# ---
	my %data = %{$_[0]};
	my $key  = $_[1];
	my $val  = $_[2];
	$val  = setupVariables ( \%data,$val );
	if (! defined $val) {
		return %data;
	}
	%data = HSetValue ( \%data,$key,$val );
	return %data;
}

#==========================================
# actionDoRemove
#------------------------------------------
sub actionDoRemove {
	# ...
	# This function will remove an entry or a complete
	# subtree from the data dictionary
	# ---
	my %data   = %{$_[0]};
	my $remove = $_[1];
	%data = HRemoveValue ( \%data,$remove );
	return %data;
}

#==========================================
# setupVariables
#------------------------------------------
sub setupVariables {
	# ...
	# This function will search for variables used
	# in the input stream and try to find a value for
	# the variable. If it finds such a variable it will
	# replace the contents and return the new input
	# ---
	my %data = %{$_[0]};
	my $val  = $_[1];
	while ($val =~ /(\$\{(.*?)\})/) {
		my $info = $1;
		my $variable = $2;
		my $pattern  = quotemeta $info;
		my $index = -1;
		if ($variable =~ /(.*)\[(\d+)\]/) {
			$variable = $1;
			$index = $2;
		}
		$variable = lookupVariable ( $variable );
		if (! defined $variable) {
			print STDERR ("variable lookup failed: no such variable $info\n");
			return undef;
		}
		if ($index != -1) {
			my @list = split (/,/,$variable);
			$variable = $list[$index];
		}
		$val =~ s/$pattern/$variable/;
	}
	return $val;
}

#==========================================
# actionReadTree
#------------------------------------------
sub actionReadTree {
	# ...
	# This function is called if the user wants to read
	# data from the data dictionary. The result is shown
	# as a tree
	# ---
	my %data = %{$_[0]};
	my $search = $_[1];

	$rcount++;

	my $value = HGetValue(\%data,$search);
	if ((! defined $value) || ($value eq "")) {
		my $ref = HGetValue (
			\%data,$search,"GET_REF"
		);
		if ((defined $ref) && ($ref ne "")) {
			my $count = 0;
			my @keylist = sort keys %{$ref};
			my $ksize = @keylist - 1;
			if ($ksize < 0) {
				printf "\n";
			}
			foreach my $k (@keylist) {
				my $ref = HGetValue (\%data,"$search->$k","GET_REF");
				if ((defined $ref) && ($ref ne "")) {
					my $space = " ";
					$space x= $indent;
					$space = replace (\@rsteps,$space);
					if ($count == 0) {
						if (($count == 0) && ($rcount == 1)) {
							my $nextsize = keys %{$ref};
							if (($ksize == 0) && ($nextsize == 1)) {
								print "$search───$k───";
							} elsif (($ksize == 0) && ($nextsize > 1)) {
								print "$search───$k─┬─";
							} elsif (($ksize > 0) && ($nextsize > 1)) {
								print "$search─┬─$k─┬─";
							} else {
								print "$search─┬─$k───";
							}
							$indent = (length ($search) + 1);
							push (@rsteps,$indent);
							$rspace = $indent;
						} else {
							my $nextsize = keys %{$ref};
							if ($nextsize == 1) {
								print "$k───";
							} else {
								print "$k─┬─";
							}
						}
					} else {
						my $sign = "├─";
						if ($count == $ksize) {
							$sign = "└─";
							$rhash{$indent} = "done";
						}
						my $nextsize = keys %{$ref};
						if ($nextsize == 1) {
							print "$space$sign$k───";
						} elsif ($nextsize > 1) {
							print "$space$sign$k─┬─";
						} else {
							print "$space$sign$k";
						}
					}
					$indent += (length ($k) + 3);
					if ($ksize > 0) {
						push (@rsteps,$indent);
					}
					$key = $k;
					actionReadTree (\%data,"$search->$k");
				} else {
					if ($ksize == $count) {
						$rhash{$indent} = "done";
					}
					my $space = " ";
					$space x= $indent;
					$space = replace (\@rsteps,$space);
					if (($count == 0) && ($rcount == 1)) {
						if ($ksize == 0) {
							print "$search───$k\n";
						} else {
							print "$search─┬─$k\n";
						}
						$indent = (length ($search) + 1);
						push (@rsteps,$indent);
						$rspace = $indent;
					} else {
						if (($count == 0) && ($rcount > 1)) {
							print "$k\n";
						} else {
							if ($count < $ksize) {
								print "$space├─$k\n";
							} else {
								print "$space└─$k\n";
							}
						}
					}
				}
				$count++;
			}
			$rcount--;
			$indent -= (length ($key) + 3);
			pop (@rsteps);
			if ($rcount == 1) {
				$indent = $rspace;
			}
			return;
		}
	} else {
		print ("$search = $value\n");
	}
}

#==========================================
# replace
#------------------------------------------
sub replace {
	# ...
	# This is a helper function to create a complete
	# tree structure while reading data. Used only
	# in actionReadTree()
	# ---
	my @list = @{$_[0]};
	my $data = $_[1];
	my $count = 0;
	pop @list;
	foreach my $index (@list) {
		if (defined $rhash{$index}) {
			next;
		}
		my $part1 = substr ($data,0,$index+$count);
		my $part2 = substr ($data,$index+1+$count,length($data));
		$data = $part1."│".$part2;
		$count+=2;
	}
	return $data;
}

#==========================================
# actionDoCommand
#------------------------------------------
sub actionDoCommand {
	# ...
	# A command was given: evaluate the command string 
	# ---
	my $command = $_[0];
	if ($command eq "!help") {
		print "Linux SaX profile batch mode\n";
		print "(C) Copyright 2005 - SuSE Linux Products GmbH\n";
		print "\n";
		print "Basic grammar format:\n";
		print "   key -> key ->...-> key [ = ( value | \${variable} ) ]\n";
		print "   \$variable = ( string | <key sequence> )\n";
		print "\n";
		print "Commands:\n";
		print "[ !remove <sequence> ]\n";
		print "    Sequence is key path to a specific value or\n";
		print "    subtree. In case of a subtree the tree will be\n";
		print "    removed. In case of a value the value is set to\n";
		print "    an empty string\n";
		print "\n";
		print "[ !help ]\n";
		print "    This help message...\n";
		print "\n";
	}
}

#==========================================
# lookupVariable
#------------------------------------------
sub lookupVariable {
	# ...
	# This function will lookup the value for the given
	# variable. If there is no variable or no value defined
	# undef is returned
	# ---
	my $varname = "\$$_[0]";
	return $variables{$varname};
}

#==========================================
# updateVariableHash
#------------------------------------------
sub updateVariableHash {
	# ...
	# This function will create/update the variable hash
	# containing the variable=value information which is used
	# in lookupVariable() while searching for a specific
	# variable
	# ---
	my %data = %{$_[0]};
	my @varlist = @{$parser->getVariables()};
	my $varsize = @varlist;
	if ($varsize == 0) {
		return;
	}
	my $varname = $varlist[0];
	my $value   = $varlist[1];
	if ($value =~ /->/) {
		$value = HGetValue(\%data,$value);
	}
	$variables{$varname} = $value;
}

#==========================================
# clearProfile
#------------------------------------------
sub clearProfile {
	# ...
	# This function will delete the profileData list
	# ---
	@profileData = ();
	@newSections = ();
	$parser -> reload();
}

#==========================================
# unique
#------------------------------------------
sub unique {
	# ...
	# Internal function used to create unique
	# list of members
	# ---
	my @list = @_;
	my @result;
	my %map;

	sub num { $a <=> $b; };
	foreach (@list) {
	if ($_ =~ /[0-9]+/) {
		$map{$_} = $_;
	}
	}
	@result = sort num keys(%map);
	return @result;
}

1;
