#!/usr/bin/perl
# Copyright (c) 2004 SuSE AG Nuernberg, Germany.  All rights reserved.
#
# Author: Marcus Schaefer <sax@suse.de>, 2004
# libsax script: read SaXMeta data from config cache
# --
# Status: Development
#
use strict;
use Getopt::Long;
use Storable;

#--[ main ]-----#
sub main {
#-----------------------------------
# main routine checking meta data
#
	#========================================
	# get/validate options
	#----------------------------------------
	my $config = "/var/cache/sax/files/config";
	my $card   = 0;
	my $result = GetOptions(
		"card|c=i" => \$card
	);
	if ( $result != 1 ) {
		return
	}
	my $hashref = retrieve($config);
	if (! defined $hashref) {
		return
	}
	my %dialog = %{$hashref};
	foreach my $env (keys %{$dialog{SaXMeta}{$card}}) {
		print "$card : $env : $dialog{SaXMeta}{$card}{$env}\n";
	}
}

main();
