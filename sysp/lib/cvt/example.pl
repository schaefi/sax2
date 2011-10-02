#!/usr/bin/perl

use lib 'blib/arch/auto/CVT';

use CVT;

my $m = CVT::vert_refresh ( 1024, 768, 75, 0, 1, 0 );
my @r = split(/\n/,CVT::print_sax_mode ($m));

print "$r[2]\n";

