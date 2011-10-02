#!/usr/bin/perl

use lib '/usr/share/sax/profile';

use strict;
use Profile;

#====================================
# Init profile script
#------------------------------------
my $profile = ProfileInitScript();

#====================================
# get current bool options
#------------------------------------
my $import = new SaX::SaXImport ( $SaX::SAX_CARD );
$import->setSource ( $SaX::SAX_AUTO_PROBE );
$import->doImport();
my $saxCard = new SaX::SaXManipulateCard ( $import );
my %options = %{$saxCard -> getOptions()};

my @optlist = ();
foreach my $opt (keys %options) {
	if ($opt eq "NoDDC") {
		next;
	}
	if ($options{$opt} eq "") {
		push @optlist,$opt;
	}
}

#====================================
# add NoDDC option
#------------------------------------
push @optlist,"NoDDC";
my $options = join (",",@optlist);

#====================================
# Do the profile adaptions...
#------------------------------------
open (FD,">",$profile) ||
	die "NoDDC: Can't open $profile: $!";
print FD "Device -> [X] -> Option = $options\n";
close FD;
