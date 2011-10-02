#/.../
# Copyright (c) 1996 SuSE GmbH Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2000
#
# autodetect.pm configuration level 1
# provide auto detect functions using sysp
#
# CVS ID:
# --------
# Status: Up-to-date
#
use lib '/usr/share/sax/modules/detect';

use strict;

our $QueryStatus;

use FBSet;
use Card;
use Fonts;
use Keyboard;
use Layout;
use Monitor;
use Options;
use Resolution;
use Mouse;

#-----[ GetQuery ]-----#
sub GetQuery {
#------------------------------------------------
# This function call sysp -q < querystr > to     
# obtain information from the sysp prog          
# the result is a list containing the parameter  
# string in the order of the sysp output         
#
	my $querystr = $_[0];    # Query string
	my %spec     = %{$_[1]}; # specs
	my $result;              # result of sysp call
	my @list;                # result list of call 
	my $i;                   # loop counter 
	my $element;             # one element from the query string
	my @erg;                 # result list;
	my $lvalue;              # left value from regexp
	my $id;                  # identifier like "Mouse0, Card0..."             
	my @elements;            # list of identifiers
	my @line;                # one line of @list seperated through "="
	my $old;                 # old $id
	my $exit;                # exit status of query

	$QueryStatus = 0;

	$result = qx ($spec{Sysp} -q $querystr);
	if ($result =~ /.*no information.*/i) {
		$QueryStatus = -1; return; 
	}
	@list = split(/\n/,$result);
	@elements = ();
	@erg      = ();
	$old      = "";
	foreach my $n (@list) {
		@line = split(/=/,$n);
		if (! defined $line[0]) { 
			next; 
		}
		$id = $line[0];
		$id =~ s/ +//g; 
		if ($old eq $id) { 
			next;
		}
		$old = $id;  
		push (@elements,$id);
	}

	foreach my $n (sort @elements) {
	foreach $i (@list) {
		@line = split(/=/,$i);
		if (! defined $line[0]) { 
			next; 
		}
		$id = $line[0];
		$id =~ s/ +//g;   
		if ($id ne $n) {
			next;
		}

		if ($i =~ /(.*):(.*)/) {
			$lvalue  = $1;
			$element = $2;
			$element =~ s/^ +//g; 
			$element =~ s/ +$//g;
			push (@erg,$element);
			if ($querystr =~ /mouse/i) {
				$lvalue =~ s/ +//g;
				$lvalue =~ s/Mouse[0-9]//;
				$lvalue =~ s/=>//;
				push (@erg,$lvalue);
			}
		}
	}
	}
	return(@erg);
}

#-----[ GetHotQuery ]-----#
sub GetHotQuery {
#------------------------------------------------
# This function call sysp -q < querystr > to     
# obtain information from the sysp prog          
# the result is a list containing the parameter  
# and variable values as X=Y string list         
#
	my $querystr = $_[0];    # Query string
	my %spec     = %{$_[1]}; # specs

	my $result;              # result of sysp call
	my @list;                # result list of call 
	my $i;                   # loop counter 
	my %erg;                 # result hash;
	my $id;                  # identifier number              
	my @line;                # one line of @list
	my $variable;            # Variable
	my $value;               # Value
	my $count;               # Vesa mode counter
	my $exit;                # exit status of query

	$QueryStatus = 0;

	$result = qx ($spec{Sysp} -q $querystr);
	if ($result =~ /.*no information.*/i) {
		$QueryStatus = -1; return;
	} 
	@list = split(/\n/,$result);

	$count = 0;
	foreach $i (@list) {
		$i =~ /(.*)=>(.*)/;
		$id = $1;
		my @list = split (/:/,$2);
		if (! defined $id) { 
			next; 
		}
		$variable = shift (@list);
		$value = join(":",@list);
		$id =~ s/ +//g;
		if ($id !~ /^[^0-9]+([0-9]+)$/) {
			next;
		}
		$id = $1;
		$variable =~ s/ +//g;
		$value  =~ s/^ +//g;  

		if ($variable =~ /Vesa/i) {
			$erg{$id}{$variable}{$count} = $value;
			$count++;
		} else {
			$erg{$id}{$variable} = $value;
		}
	}
	return(%erg);
}

#-----[ sortres ]-----#
sub sortres {
#--------------------------------------------------
# this function sort a list of resoultion entries
# like 1024x768 ...
#
	my @list = @_;   # list of resolutions
	my %index;       # index hash

	foreach my $i (@list) {
		if ($i eq "") {
			next;
		}
		my @res   = split(/x/,$i);
		my $pixel = $res[0] * $res[1];
		$index{$pixel} = $i;
	}
	@list = ();

	sub numerisch { $b <=> $a; }
	foreach my $i (sort numerisch keys %index) {
		push(@list,$index{$i});
	}
	# /.../
	# check list do not use resolutions whose X value doesn't fit
	# into the sort order. The pixel value of X*Y is correct but
	# we don't want a virtual screen. Example:
	# 1280x1024 1440x900 -> remove 1280x1024
	# ----
	my $count = @list;
	my @list_new = ();
	for (my $i=0;$i<$count;$i++) {
		if (! $list[$i+1]) {
			push (@list_new,$list[$i]); last;
		}
		my @res_this = split(/x/,$list[$i]);
		my @res_next = split(/x/,$list[$i+1]);
		my $x_this = $res_this[0];
		my $x_next = $res_next[0];
		if ($x_this >= $x_next) {
			push (@list_new,$list[$i]);
		}
	}
	return(@list_new);
}

#-----[ numbers ]-----#
sub numbers {
#--------------------------------------------------
# sort numbers upstairs
#
    $a <=> $b;
}

1;
