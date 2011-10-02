#!/usr/bin/perl 

use lib '/usr/share/sax/modules';

use SPPParse;
use Storable;

my $config = "/var/cache/sax/files/config";
my $hashref = retrieve($config);
my %data = %{$hashref};

prepareProfile ("/usr/share/sax/profile/alps");

%data = includeProfile (\%data);


clearProfile();

%data = includeProfile (\%data);
